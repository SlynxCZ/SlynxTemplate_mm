# SlynxTemplate

**Starting point for a CS2 Metamod:Source plugin.**

Builds as-is with CMake and AMBuild, ships a working CI that releases Linux and
Windows binaries on a tag, and carries a private SourceHook so the plugin's hooks
don't share metamod's engine.

## Names

Renaming is four places. `slynx_template` is what the binary, the `addons/`
folder and the VDF alias are called; `SlynxTemplate_mm` is only the repo and the
release artifacts.

| Where | Change |
| --- | --- |
| `CMakeLists.txt` | `project()` and both `LIBRARY_OUTPUT_DIRECTORY` paths |
| `AMBuildScript` | `plugin_name` and `plugin_alias` defaults |
| `makefiles/metamod/` | rename `slynx_template.vdf.in`, fix the alias/file inside it and the two paths in `configure_metamod.cmake` |
| `.github/workflows/main.yml` | artifact names and the release tarball names |

`PackageScript` and `docker/` derive everything from `MMSPlugin.plugin_name`, so
they need nothing.

Then `src/plugin.cpp`'s `GetName()` / `GetLogTag()` / `GetDescription()`, and the
include guard in `src/plugin.h`.

## What's already wired up

`Load()` grabs the usual engine interfaces and installs one hook on
`INetworkServerService::StartupServer`, which fires on every map change and is
the natural place to reset per-map state. `Unload()` removes it. That is the
whole plugin -- it is there as a shape to copy, not because it does anything.

`src/sdk/CServerSideClient.h` is a hand-reconstructed layout of the engine-side
client. Nothing includes it by default. It is here because it is the piece that
costs the most to rebuild from scratch and the one most plugins end up needing;
delete it if yours doesn't. It is version-dependent and does not fail loudly --
on the current build `m_nSignonState` sits at offset 100 on Linux, 92 on Windows.
Worth re-checking against CS2Fixes after a major game update.

## Hooking recipes

**An interface you can fetch.** Fetch it, then `SH_ADD_HOOK` on the pointer:

```cpp
SH_DECL_HOOK3_void(ICvar, DispatchConCommand, SH_NOATTRIB, 0, ConCommandRef, const CCommandContext&, const CCommand&);

m_iHookID = SH_ADD_HOOK(ICvar, DispatchConCommand, g_pCVar,
    SH_MEMBER(this, &Plugin::Hook_DispatchConCommand), false);
```

**An engine class with no interface and no instance at load time**
(`CServerSideClient` and friends). Resolve the vtable out of the module by name
and hook it directly -- this is what `vendor/dynlibutils` is for:

```cpp
CModule libengine(g_pEngineServer);
CMemory pVTable = libengine.GetVirtualTableByName("CServerSideClient");
if (!pVTable.IsValid())
{
    std::snprintf(error, maxlen, "Failed to find the CServerSideClient vtable");
    return false;
}

m_iHookID = SH_ADD_DVPHOOK(CServerSideClient, SendNetMessage,
    pVTable.RCast<CServerSideClient *>(),
    SH_MEMBER(this, &Plugin::Hook_SendNetMessage), false);
```

`SH_ADD_DVPHOOK` (`Hook_DVP`) takes its pointer as the vtable itself rather than
as an object and leaves the hook's interface pointer null, so one install covers
every instance -- no waiting for the first client, no per-object bookkeeping.
`META_IFACEPTR(CServerSideClient)` inside the handler still gives you the real
`this`, which is how you tell instances apart.

Check the hook ID either way. A zero means the hook never went in, and a plugin
that silently does nothing is worse than one that refuses to load.

## Protobufs

`makefiles/protobuf.cmake` and `AMBuilder` each carry a list of `.proto` files to
generate. If you need a message that isn't compiling, it is almost always because
its file is missing from **both** lists.

The CMake path regenerates when a `.proto` changes only because of the `DEPENDS`
on the custom command -- without it you silently keep building against a stale
copy of the schema, which is a bad afternoon. AMBuild tracks this itself.

## Building

Requires `HL2SDKCS2`, `MMSOURCE_DEV` and `CSGO_PROTO` in the environment, and the
submodules checked out (`git submodule update --init --recursive`).

CMake (local dev):

```
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCMAKE_C_COMPILER=clang-18 -DCMAKE_CXX_COMPILER=clang++-18
cmake --build build
```

Output lands in `build/addons/`, laid out ready to copy into the server's
`game/csgo/` directory.

AMBuild (what CI and the docker image run):

```
docker compose -f docker/docker-compose.yml up --build --abort-on-container-exit
```

Packaged output lands in `build/package/cs2`.

CI (`.github/workflows/main.yml`) builds Linux in the steamrt sniper container
and Windows with MSVC + AMBuild, and on a tag pushes
`SlynxTemplate_mm_<tag>-{linux,windows}.tar.gz` to the release.

## SourceHook

This plugin carries its **own private SourceHook** (`vendor/sourcehook`, wired up
via `sourcehook_metamod_override.h`) rather than using metamod's shared instance.

That is free right up until a hook here lands on a vtable slot another plugin on
the same server also hooks through metamod's shared engine (CounterStrikeSharp,
CS2Fixes). Two engines patching one slot do not coordinate: whichever patched
last owns it, each treats what it found there as "the original", and
`MRES_SUPERCEDE` only suppresses its own chain. Ordering then comes down to
plugin load order, and unloading is order-sensitive too.

`IGameEventSystem::PostEventAbstract` and `ICvar::DispatchConCommand` are the
crowded slots. If you hook one of those and it bites, the fix is to put both
sides on one engine -- back to metamod's shared SourceHook, or have the other
side join this one via `sourcehook_metamod_shared.h` -- not to change the hook.
