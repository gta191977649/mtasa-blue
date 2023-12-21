/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaCameraDefs.cpp
 *  PURPOSE:     Lua camera definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CCutsceneManager.h>

typedef void (*LoadCutsceneDataFunc)(const char*);
typedef void (*StartCutsceneFunc)();
// Declare function pointers
LoadCutsceneDataFunc LoadCutsceneDataPtr;
StartCutsceneFunc    StartCutscenePtr;

CLuaCutsceneDefs::CLuaCutsceneDefs()
{
    // Assign the function pointers to the specific addresses
    LoadCutsceneDataPtr = reinterpret_cast<LoadCutsceneDataFunc>(0x4D5E80);
    StartCutscenePtr = reinterpret_cast<StartCutsceneFunc>(0x5B1460);
}


void CLuaCutsceneDefs::LoadFunctions()
{
    // Backwards compatibility functions
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"loadCutscene", ArgumentParser<LoadCutscene>},

    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaCutsceneDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    //lua_classfunction(luaVM, "setApplication", "setDiscordApplicationID");


    lua_registerclass(luaVM, "Cutscene");
}
bool CLuaCutsceneDefs::LoadCutscene(std::string cutsceneName)
{


    //CCutsceneManager::LoadCutsceneData(cutsceneName);
    if (LoadCutsceneDataPtr)
    {
        //LoadCutsceneDataPtr(cutsceneName);
    }
    return true;
}

