
#include "AudioManager.h"

void AudioManager::Init()
{
    InitAudioDevice();
}

void AudioManager::PlayBackgroundMusic(const std::string& filePath)
{
    if (_isMusicLoaded)
    {
        UnloadMusicStream(_bgMusic);
    }

    _bgMusic = LoadMusicStream(filePath.c_str());

    _bgMusic.looping = true;

    PlayMusicStream(_bgMusic);
    _isMusicLoaded = true;
}

void AudioManager::Update()
{
    if (_isMusicLoaded)
    {
        UpdateMusicStream(_bgMusic);
    }
}

void AudioManager::Close()
{
    if (_isMusicLoaded)
    {
        UnloadMusicStream(_bgMusic);
        _isMusicLoaded = false;
    }
    CloseAudioDevice();
}