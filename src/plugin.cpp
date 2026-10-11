/**
 * vim: set ts=4 sw=4 tw=99 noet:
 * =============================================================================
 * SlynxTemplate_mm
 * Copyright (C) 2026 Michal "Slynx (˙·٠● S l y n x ●٠·˙)" Přikryl.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   - Michal "Slynx (˙·٠● S l y n x ●٠·˙)" Přikryl
 *
 * Project: SlynxTemplate_mm
 */

#include "plugin.h"

#include "utils.hpp"
#include "module.hpp"

#include <engine2/inetworkserverservice.h>
#include <engine2/isource2server.h>
#include <engine2/ivengineserver2.h>
#include <entitysystem.h>
#include <filesystem/ifilesystem.h>
#include <networksystem/inetworksystem.h>
#include <playerslot.h>
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
