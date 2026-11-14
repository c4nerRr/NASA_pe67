#ifndef NASA_SETTING_H
#define NASA_SETTING_H
#include "IState.h"
#include "raylib.h"

class Setting : public IState
{
private:
    float volume = 1.0f;
    bool isSoundOn = true;
    bool isDragging = false;
    float prevVolume = 1.0f;
    bool prevSoundOn = true;

    Rectangle panelRec{};
    Rectangle toggleBtnRec{};
    Rectangle sliderBarRec{};
    Rectangle sliderHandleRec{};
    Rectangle btnApplyRec{};
    Rectangle btnBackRec{};

    bool isLoaded = false;

    Texture2D bgTexture{};
    Texture2D panelTexture{};
    Texture2D btnApplyTexture{};
    Texture2D btnBackTexture{};
    Texture2D sliderBarTexture{};
    Texture2D sliderHandleTexture{};

public:
    Setting();
    ~Setting() override;
    void OnEnter() override;
    void Update(float deltaTime) override;
    void Draw() const override;
};


#endif //NASA_SETTING_H