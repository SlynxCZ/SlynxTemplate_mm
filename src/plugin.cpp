// Author: Michal Přikryl (Slynx) <github.com/SlynxCZ>

#include "plugin.h"

#include "utils.hpp"
#include "module.hpp"

#include "eiface.h"
#include "entitysystem.h"
#include "iserver.h"
#include "playerslot.h"
#include "interfaces/interfaces.h"

#include <cstdio>

#define VERSION_STRING SEMVER " @ " GITHUB_SHA
#define BUILD_TIMESTAMP __DATE__ " " __TIME__

using namespace DynLibUtils;

Plugin g_Plugin;
PLUGIN_EXPOSE(Plugin, g_Plugin);

bool Plugin::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();
	SH_METAMOD_OVERRIDE_SAVEVARS(id);

	GET_V_IFACE_CURRENT(GetServerFactory, g_pSource2Server, ISource2Server, SOURCE2SERVER_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pEngineServer, IVEngineServer2, SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pSchemaSystem, ISchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pGameResourceServiceServer, IGameResourceService, GAMERESOURCESERVICESERVER_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pNetworkServerService, INetworkServerService, NETWORKSERVERSERVICE_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pNetworkSystem, INetworkSystem, NETWORKSYSTEM_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetFileSystemFactory, g_pFullFileSystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION);

	CModule libserver(g_pSource2Server);
	CModule libengine(g_pEngineServer);

	return true;
}

bool Plugin::Unload(char* error, size_t maxlen)
{
	return true;
}

///////////////////////////////////////

CGameEntitySystem* GameEntitySystem()
{
	// CGameResourceService::SetEntityResourceManifest
	// str server_entities
	return *CMemory(g_pGameResourceServiceServer).Offset(WIN_LINUX(0x58, 0x50)).RCast<CGameEntitySystem**>();
}

///////////////////////////////////////
const char* Plugin::GetLicense()
{
	return "GPLv3";
}

const char* Plugin::GetVersion()
{
	return VERSION_STRING;
}

const char* Plugin::GetDate()
{
	return BUILD_TIMESTAMP;
}

const char* Plugin::GetLogTag()
{
	return "SlynxTemplate";
}

const char* Plugin::GetAuthor()
{
	return "Slynx (˙·٠● S l y n x ●٠·˙)";
}

const char* Plugin::GetDescription()
{
	return "Metamod plugin template";
}

const char* Plugin::GetName()
{
	return "Slynx template";
}

const char* Plugin::GetURL()
{
	return "https://slynxdev.cz";
}
