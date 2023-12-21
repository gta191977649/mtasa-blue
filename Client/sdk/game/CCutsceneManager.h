/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CCutsceneManager.h
 *  PURPOSE:     Camera entity interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CCutsceneManager
{
public:
    enum class LoadStatus
    {
        NOT_LOADED,
        LOADING,
        LOADED,
    };

    enum class PlayStatus
    {
        S0,
        STARTING = 1,
        S2,
        S3,
        S4
    };

public:

    virtual void LoadCutsceneData(const char* cutsceneName) = 0;
    virtual void StartCutscene() = 0;
};
