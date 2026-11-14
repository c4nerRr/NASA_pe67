
#include "Setting.h"
#include "Setting.h"
#include <string>
#include "GameManager.h"
#include "InputManager.h"

Setting::Setting() = default;

Setting::~Setting()
{
    UnloadTexture(bgTexture);
    UnloadTexture(panelTexture);
    UnloadTexture(btnApplyTexture);
    UnloadTexture(btnBackTexture);
    UnloadTexture(sliderBarTexture);
    UnloadTexture(sliderHandleTexture);
}

void Setting::OnEnter()
{
    if (!isLoaded)
    {
        bgTexture = LoadTexture("assets/settings/settings.png");
        panelTexture = LoadTexture("assets/settings/settings_box.png");
        btnApplyTexture = LoadTexture("assets/settings/apply.png");
        btnBackTexture = LoadTexture("assets/settings/goback.png");
        sliderBarTexture = LoadTexture("assets/settings/drag_sound.png");
        sliderHandleTexture = LoadTexture("assets/settings/click_sound.png");
        isLoaded = true;
    }

    float sw = GetScreenWidth();
    float sh = GetScreenHeight();

    panelRec = { sw / 2 - 200, sh / 2 - 150, 400, 300 };
    toggleBtnRec = { panelRec.x + 50, panelRec.y + 40, 150, 40 };
    sliderBarRec = { panelRec.x + 50, panelRec.y + 150, 300, 15 };

    float handleWidth = 20;
    sliderHandleRec = { sliderBarRec.x + (sliderBarRec.width - handleWidth) * volume, sliderBarRec.y - 15, handleWidth, 45 };

    btnApplyRec = { panelRec.x, panelRec.y + panelRec.height + 20, 150, 50 };
    btnBackRec = { panelRec.x + 250, panelRec.y + panelRec.height + 20, 150, 50 };

    prevVolume = volume;
    prevSoundOn = isSoundOn;
}

void Setting::Update(float deltaTime)
{
    Vector2 mousePos = GetMousePosition();

    if (CheckCollisionPointRec(mousePos, toggleBtnRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        isSoundOn = !isSoundOn;
        SetMasterVolume(isSoundOn ? volume : 0.0f);
    }

    Rectangle dragArea = { sliderBarRec.x - 20, sliderBarRec.y - 20, sliderBarRec.width + 40, sliderBarRec.height + 40 };
    if (CheckCollisionPointRec(mousePos, dragArea) && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        sliderHandleRec.x = mousePos.x - (sliderHandleRec.width / 2.0f);

        if (sliderHandleRec.x < sliderBarRec.x)
        {
            sliderHandleRec.x = sliderBarRec.x;
        }
        if (sliderHandleRec.x > sliderBarRec.x + sliderBarRec.width - sliderHandleRec.width)
        {
            sliderHandleRec.x = sliderBarRec.x + sliderBarRec.width - sliderHandleRec.width;
        }

        volume = (sliderHandleRec.x - sliderBarRec.x) / (sliderBarRec.width - sliderHandleRec.width);

        if (isSoundOn)
        {
            SetMasterVolume(volume);
        }
    }

    if (CheckCollisionPointRec(mousePos, btnApplyRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        GameManager::Instanse().ChangeState(GameState::Menu);
    }

    if (CheckCollisionPointRec(mousePos, btnBackRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        volume = prevVolume;
        isSoundOn = prevSoundOn;

        SetMasterVolume(isSoundOn ? volume : 0.0f);

        sliderHandleRec.x = sliderBarRec.x + (sliderBarRec.width - sliderHandleRec.width) * volume;

        GameManager::Instanse().ChangeState(GameState::Menu);
    }
}

void Setting::Draw() const
{
    Rectangle screenRec = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
    DrawTexturePro(bgTexture, { 0, 0, (float)bgTexture.width, (float)bgTexture.height }, screenRec, { 0, 0 }, 0.0f, WHITE);

    DrawTexturePro(panelTexture, { 0, 0, (float)panelTexture.width, (float)panelTexture.height }, panelRec, { 0, 0 }, 0.0f, WHITE);

    std::string toggleText = isSoundOn ? "On/Off: ON" : "On/Off: OFF";
    DrawText(toggleText.c_str(), toggleBtnRec.x, toggleBtnRec.y, 30, DARKBLUE);

    DrawTexturePro(sliderBarTexture, { 0, 0, (float)sliderBarTexture.width, (float)sliderBarTexture.height }, sliderBarRec, { 0, 0 }, 0.0f, WHITE);

    DrawTexturePro(sliderHandleTexture, { 0, 0, (float)sliderHandleTexture.width, (float)sliderHandleTexture.height }, sliderHandleRec, { 0, 0 }, 0.0f, WHITE);

    std::string volText = std::to_string((int)(volume * 100)) + "%";
    DrawText(volText.c_str(), sliderBarRec.x + sliderBarRec.width + 15, sliderBarRec.y - 5, 20, DARKBLUE);

    Color applyTint = CheckCollisionPointRec(GetMousePosition(), btnApplyRec) ? GRAY : WHITE;
    DrawTexturePro(btnApplyTexture, { 0, 0, (float)btnApplyTexture.width, (float)btnApplyTexture.height }, btnApplyRec, { 0, 0 }, 0.0f, applyTint);

    Color backTint = CheckCollisionPointRec(GetMousePosition(), btnBackRec) ? GRAY : WHITE;
    DrawTexturePro(btnBackTexture, { 0, 0, (float)btnBackTexture.width, (float)btnBackTexture.height }, btnBackRec, { 0, 0 }, 0.0f, backTint);
}