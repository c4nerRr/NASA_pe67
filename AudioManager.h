#pragma once
#include "raylib.h"
#include <string>

class AudioManager {
public:
    AudioManager() = default;
    ~AudioManager() = default;

    static AudioManager& Instance()
    {
        static AudioManager instance;
        return instance;
    }

    void Init();
    void PlayBackgroundMusic(const std::string& filePath);
    void Update();
    void Close();

private:
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    Music _bgMusic{};
    bool _isMusicLoaded = false;
};