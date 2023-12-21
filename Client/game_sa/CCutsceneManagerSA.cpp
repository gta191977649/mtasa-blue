#include "StdInc.h"
#include "CCutsceneManagerSA.h"

// Define function pointer types
typedef void (*LoadCutsceneDataFunc)(const char*);
typedef void (*StartCutsceneFunc)();

// Declare function pointers
LoadCutsceneDataFunc LoadCutsceneDataPtr;
StartCutsceneFunc    StartCutscenePtr;

// Constructor or initialization function
CCutsceneManagerSA::CCutsceneManagerSA()
{
    // Assign the function pointers to the specific addresses
    LoadCutsceneDataPtr = reinterpret_cast<LoadCutsceneDataFunc>(0x4D5E80);
    StartCutscenePtr = reinterpret_cast<StartCutsceneFunc>(0x5B1460);
}

// Implementation of LoadCutsceneData
void CCutsceneManagerSA::LoadCutsceneData(const char* cutsceneName)
{
    if (LoadCutsceneDataPtr)
    {
        LoadCutsceneDataPtr(cutsceneName);
    }
}

// Implementation of StartCutscene
void CCutsceneManagerSA::StartCutscene()
{
    if (StartCutscenePtr)
    {
        StartCutscenePtr();
    }
}
