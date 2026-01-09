#include "engine/SoundEngine.hpp"
#include <ik_ISoundEngine.h>
#include <irrKlang.h>
#include <stdexcept>

using namespace irrklang;

SoundEngine::SoundEngine()
    : engine(nullptr)
    , music(nullptr)
{
    engine = createIrrKlangDevice();
    if (!engine)
        throw std::runtime_error("Failed to create irrKlang sound engine");

    engine->setSoundVolume(0.5f);
}

SoundEngine::~SoundEngine()
{
    stopMusic();
    if(engine) engine->drop();
}

void SoundEngine::playMusic(const std::string& path, bool loop)
{
    stopMusic();

    music = engine->play2D(path.c_str(), loop, false, true);
    
}

void SoundEngine::stopMusic()
{
    if (music)
    {
        music->stop();
        music->drop();
        music = nullptr;
    }
}

void SoundEngine::playSFX(const std::string& path)
{
    engine->play2D(path.c_str(), false);
}

void SoundEngine::setMasterVolume(float volume)
{
    engine->setSoundVolume(volume);
}
