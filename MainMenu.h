//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_MAINMENU_H
#define NASA_MAINMENU_H
#include "IState.h"
#include "raylib.h"

class MainMenu : public IState {
    struct LayOut {
        Rectangle play{};
        Rectangle options{};
        Rectangle exit{};
    };

    [[nodiscard]]LayOut GetLayOut() const;
    void DrawButton(Rectangle bounds, const char* text ) const;

    public:
        MainMenu();
        ~MainMenu() override;

        void Update(float dt) override;
        void Draw() const override;
};


#endif //NASA_MAINMENU_H