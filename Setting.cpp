
#include "Setting.h"
#include "Setting.h"
#include <string>
#include "GameManager.h"
#include "InputManager.h"
#include <array>
#include "FontManager.h"

namespace {
    constexpr std::array<const char*, 3> developerCredits{
        "Lead Programmer: Shyrokopoias Veronika",
        "Game Designer: Yasinska Yuliia",
        "Artist: Kucheruk Liliia"
    };

    constexpr Color creditsTitleColor{218, 191, 112, 255};
    constexpr Color creditsTextColor{177, 181, 204, 255};
}

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
        bgTexture = LoadTexture("assets/settings/settings_old.png");
        panelTexture = LoadTexture("assets/settings/settings_box.png");
        btnApplyTexture = LoadTexture("assets/settings/apply.png");
        btnBackTexture = LoadTexture("assets/settings/goback.png");
        sliderBarTexture = LoadTexture("assets/settings/Line_7.png");
        sliderHandleTexture = LoadTexture("assets/settings/click_sound.png");
        isLoaded = true;
    }

    float sw = GetScreenWidth();
    float sh = GetScreenHeight();

    panelRec = { sw / 2 - 240, sh / 2 - 180, 480, 360 };

    toggleBtnRec = { panelRec.x + 60, panelRec.y + 70, 200, 40 };
    const float sliderWidth = panelRec.width * 0.58f;
    const float sliderHeight = sliderWidth * static_cast<float>(sliderBarTexture.height) / sliderBarTexture.width;
    sliderBarRec = {panelRec.x + 60, panelRec.y + 200, sliderWidth, sliderHeight};

    float handleWidth = 24;
    sliderHandleRec = { sliderBarRec.x + (sliderBarRec.width - handleWidth) * volume, sliderBarRec.y - 18, handleWidth, 54 };

    const float btnW = panelRec.width * 0.415f;
    const float btnH = btnW * static_cast<float>(btnApplyTexture.height) / btnApplyTexture.width;

    btnBackRec = { panelRec.x + 27, panelRec.y + panelRec.height + 25, btnW, btnH };
    btnApplyRec = { panelRec.x + panelRec.width - btnW - 27, panelRec.y + panelRec.height + 25, btnW, btnH };

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
    ClearBackground(BLACK);

    Rectangle screenRec = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
    DrawTexturePro(bgTexture, { 0, 0, (float)bgTexture.width, (float)bgTexture.height }, screenRec, { 0, 0 }, 0.0f, WHITE);

    DrawTexturePro(panelTexture, { 0, 0, (float)panelTexture.width, (float)panelTexture.height }, panelRec, { 0, 0 }, 0.0f, WHITE);

    std::string toggleText = isSoundOn ? "On/Off: ON" : "On/Off: OFF";
    DrawText(toggleText.c_str(), toggleBtnRec.x, toggleBtnRec.y, 36, DARKBLUE);

    DrawTexturePro(sliderBarTexture, { 0, 0, (float)sliderBarTexture.width, (float)sliderBarTexture.height }, sliderBarRec, { 0, 0 }, 0.0f, WHITE);
    DrawTexturePro(sliderHandleTexture, { 0, 0, (float)sliderHandleTexture.width, (float)sliderHandleTexture.height }, sliderHandleRec, { 0, 0 }, 0.0f, WHITE);

    std::string volText = std::to_string((int)(volume * 100)) + "%";
    DrawText(volText.c_str(), sliderBarRec.x + sliderBarRec.width + 20, sliderBarRec.y - 5, 24, DARKBLUE);

    Color backTint = CheckCollisionPointRec(GetMousePosition(), btnBackRec) ? GRAY : WHITE;
    DrawTexturePro(btnBackTexture, { 0, 0, (float)btnBackTexture.width, (float)btnBackTexture.height }, btnBackRec, { 0, 0 }, 0.0f, backTint);

    Color applyTint = CheckCollisionPointRec(GetMousePosition(), btnApplyRec) ? GRAY : WHITE;
    DrawTexturePro(btnApplyTexture, { 0, 0, (float)btnApplyTexture.width, (float)btnApplyTexture.height }, btnApplyRec, { 0, 0 }, 0.0f, applyTint);
    DrawCredits();
}

void Setting::DrawCredits() const {
    const float centerX = panelRec.x + panelRec.width * 0.5f;
    float fontSize = panelRec.width * 0.04f;

    for (const char* text : developerCredits) {
        const float width = FontManager::Measure(text, fontSize).x;
        if (width > panelRec.width) fontSize *= panelRec.width / width;
    }

    const float lineHeight = fontSize * 1.5f;
    float y = btnApplyRec.y + btnApplyRec.height * 1.4f;

    FontManager::DrawCentered("DEVELOPERS", centerX, y, fontSize, creditsTitleColor);
    y += lineHeight * 1.25f;

    for (const char* text : developerCredits) {
        FontManager::DrawCentered(text, centerX, y, fontSize, creditsTextColor);
        y += lineHeight;
    }
}
