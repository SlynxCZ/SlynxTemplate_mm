# SlynxTemplate

**Starting point for a CS2 Metamod:Source plugin.**

Builds as-is with CMake and AMBuild, ships CI that releases Linux and Windows
binaries on a tag, carries a private SourceHook so the plugin's hooks don't share
metamod's engine, and comes with schema field access already working.

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

## What's in the box

`Load()` fetches the interfaces a plugin usually ends up wanting -- server,
engine, cvar, schema system, game resource service, network server service,
network system, filesystem -- and opens `CModule` handles on `server` and
`engine`. `Unload()` is empty. There are no hooks; `plugin.h` has an empty
`private: // Hooks` section waiting for them.

### GameEntitySystem()

The SDK declares `extern CGameEntitySystem *GameEntitySystem();` in
`entity2/entitysystem.h` but never defines it -- that is the plugin's job.
`src/plugin.cpp` does it by reaching into the game resource service:

```cpp
CGameEntitySystem *GameEntitySystem()
{
    // CGameResourceService::SetEntityResourceManifest
    // str server_entities
    return *CMemory(g_pGameResourceServiceServer).Offset(WIN_LINUX(0x58, 0x50)).RCast<CGameEntitySystem **>();
}
```

That offset is version-dependent and fails silently if the game moves it. The
comment above it is the recipe for finding it again.

### Schema fields

`src/sdk/schemasystem_helper.h` gives you `SCHEMA_FIELD` and
`SCHEMA_FIELD_POINTER`. Declare a field on a class and you get an accessor plus
netvar replication:

```cpp
class CCSPlayerController
{
public:
    SCHEMA_FIELD(int32_t, CCSPlayerController, m_iPawnArmor)
};

pController->m_iPawnArmor() = 50;
pController->m_iPawnArmor.NetworkStateChanged();
```

`GetServerPropInfo()` looks the offset up through the schema system's
`libserver.so` / `server.dll` type scope, and separately asks the network
serializer database whether the field is replicated at all. Plenty of schema
fields never leave the server, and `NetworkStateChanged()` is a no-op for those
rather than wasted work.

Fields on chained classes (`CCSGameRules` and friends) route the notification
through the class' `CNetworkVarChainer` so the change gets attributed to the
owning entity; `GetServerChainOffset()` walks up base classes to find it. Fields
straight on a `CEntityInstance` notify the entity directly. The macro picks
whichever applies, so calling code doesn't have to care.

A missing class or field is an `Error()` at first use, not a silent zero offset.

### utils.hpp

`WIN_LINUX(win, linux)` for platform-split constants, `CallVFunc<T, index>()` for
calling a vtable slot you don't have a header for, and a compile-time djb2a
`"..."_sh` literal for switching on strings.

## Adding a hook

**An interface you can fetch.** Fetch it, then `SH_ADD_HOOK` on the pointer:

```cpp
SH_DECL_HOOK3_void(ICvar, DispatchConCommand, SH_NOATTRIB, 0, ConCommandRef, const CCommandContext&, const CCommand&);

m_iHookID = SH_ADD_HOOK(ICvar, DispatchConCommand, g_pCVar,
    SH_MEMBER(this, &Plugin::Hook_DispatchConCommand), false);
```

**An engine class with no interface and no instance at load time**
(`CServerSideClient` and friends). Resolve the vtable out of the module by name
and hook it directly -- this is what the `CModule` handles in `Load()` and
`vendor/dynlibutils` are for:

```cpp
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
`this`, which is how you tell instances apart. This needs a header for the class
you're hooking, since SourceHook derives the vtable index from the declared
virtual order.

Check the hook ID either way, and store it so `Unload()` can
`SH_REMOVE_HOOK_ID()` it. A zero means the hook never went in, and a plugin that
silently does nothing is worse than one that refuses to load.

Some hook prototypes need a type the SDK only forward-declares --
`GameSessionConfiguration_t` is the usual one. Defining an empty class of that
name in your `.cpp` is enough to complete it.

## Source lists

Two of them, and they have to agree:

- `CMakeLists.txt` globs `src/` and appends the SDK's `memoverride.cpp`,
  `bitbuf.cpp`, `convar.cpp`, `keyvalues3.cpp` and the three `entity2/` files.
- `AMBuilder` lists `src/` files **explicitly** -- a new `.cpp` under `src/` gets
  picked up by CMake automatically and ignored by AMBuild until you add it. The
  SDK half lives in `AMBuildScript`'s `HL2Library()`.

A file that builds locally and fails to link in CI is almost always this.

## Protobufs

`makefiles/protobuf.cmake` and `AMBuilder` each carry a list of `.proto` files to
generate. If a message won't compile, it is almost always because its file is
missing from **both** lists.

The CMake path regenerates when a `.proto` changes only because of the `DEPENDS`
on the custom command -- without it you silently keep building against a stale
copy of the schema, which is a bad afternoon. AMBuild tracks this itself.

## Building

Requires `S2SDK` (or `HL2SDKCS2`), `MMSOURCE_DEV` and `CSGO_PROTO` in the environment, and the
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
