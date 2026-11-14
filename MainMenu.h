
#ifndef NASA_MAINMENU_H
#define NASA_MAINMENU_H
#include "IState.h"
#include "MainMenuConfig.h"
#include "raylib.h"

class MainMenu : public IState {
    struct ButtonTextures {
        Texture2D normal{};
        Texture2D hover{};
    };

    MainMenuConfig _config;
    Texture2D _background{};
    std::vector<ButtonTextures> _buttons;
    bool _loaded = false;

    [[nodiscard]] Rectangle GetCanvas() const;

    [[nodiscard]] Rectangle GetButtonBounds(
        std::size_t index,
        Rectangle canvas
    ) const;

public:
    explicit MainMenu(MainMenuConfig config = {});
    ~MainMenu() override;

    MainMenu(const MainMenu&) = delete;
    MainMenu& operator=(const MainMenu&) = delete;

    void OnEnter() override;
    void Update(float dt) override;
    void Draw() const override;
};


#endif //NASA_MAINMENU_H