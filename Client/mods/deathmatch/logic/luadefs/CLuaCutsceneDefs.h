/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaBlipDefs.h
 *  PURPOSE:     Lua blip definitions class header
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaCutsceneDefs : public CLuaDefs
{
public:
    CLuaCutsceneDefs();
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);
    static bool LoadCutscene(std::string cutsceneName);
    static bool StartCutscene();
};
