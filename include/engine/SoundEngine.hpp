#pragma once

#include <ik_ISound.h>
#include <ik_ISoundEngine.h>
#include <irrKlang.h>
#include <string>

class SoundEngine
{
public:
    SoundEngine();
    ~SoundEngine();


    void playMusic(const std::string& path, bool loop = true);
    void stopMusic();

    void playSFX(const std::string& path);
    void setMasterVolume(float volume);

private:


    irrklang::ISoundEngine *engine;
    irrklang::ISound *music;
};
