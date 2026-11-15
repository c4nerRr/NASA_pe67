
#ifndef NASA_PAUSEMENU_H
#define NASA_PAUSEMENU_H
#include "IState.h"
#include "PauseArt.h"
#include "raylib.h"


class PauseMenu : public IState {

    struct Layout {
        Rectangle panel{};
        Rectangle resume{};
        Rectangle menu{};
        Rectangle quit{};

        float scale = 1.0f;
    };

    PauseArt _art;

    [[nodiscard]] Layout GetLayout() const;

    void DrawButton(Rectangle bounds, PauseArt::Button button) const;

    public:

    PauseMenu();
    ~PauseMenu() override;

    void OnEnter() override;
    void Update(float dt) override;
    void Draw() const override;
};


#endif //NASA_PAUSEMENU_H