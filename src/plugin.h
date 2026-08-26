#ifndef _INCLUDE_SLYNX_TEMPLATE_PLUGIN_SLYNX_H_
#define _INCLUDE_SLYNX_TEMPLATE_PLUGIN_SLYNX_H_
#ifdef _WIN32
#pragma once
#endif

#include "inetchannel.h"
#include "network_connection.pb.h"
#include <ISmmPlugin.h>

// Redirects SH_GLOB_SHPTR/SH_GLOB_PLUGPTR onto a private, plugin-owned
// SourceHook engine (vendor/sourcehook) instead of metamod's shared
// g_SHPtr/g_PLID -- must come after ISmmPlugin.h (which is what defines the
// defaults this overrides) and before any SH_DECL_HOOK*/SH_ADD_*HOOK usage.
#include "sourcehook/sourcehook_metamod_override.h"

class CNetMessage;
class GameSessionConfiguration_t;
class ISource2WorldSession;

class Plugin final : public ISmmPlugin
{
public:
	bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late) override;
	bool Unload(char* error, size_t maxlen) override;

private: // Hooks

private:
	const char* GetAuthor() override;
	const char* GetName() override;
	const char* GetDescription() override;
	const char* GetURL() override;
	const char* GetLicense() override;
	const char* GetVersion() override;
	const char* GetDate() override;
	const char* GetLogTag() override;
};

extern Plugin g_Plugin;

PLUGIN_GLOBALVARS();

#endif // _INCLUDE_SLYNX_TEMPLATE_PLUGIN_SLYNX_H_
