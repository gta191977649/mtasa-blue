#include <game/Common.h>
#include <game/CCutsceneManager.h>

#define FUNC_LoadCutsceneData 0x4D5E80
#define FUNC_StartCutscene    0x5B1460

class CCutsceneManagerSA : public CCutsceneManager
{

public:
    CCutsceneManagerSA();
    void LoadCutsceneData(const char* cutsceneName);
    void StartCutscene();
};
