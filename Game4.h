
#ifndef NASA_GAME4_H
#define NASA_GAME4_H
#include "IGame.h"
#include "IState.h"
#include <array>

#include "GardenArt.h"
#include "raylib.h"
#include "GreenGarden.h"


class Game4 : public IGame, public IState {
public:
    Game4();
    ~Game4() override;

    void OnEnter() override;
    void Update(float dt) override;
    void Draw() const override;

    [[nodiscard]] int GetReputation() const;

private:
    enum class Tool {
        None,
        Water,
        Light
    };

    enum class Phase {
        Theory,
        Rules,
        Playing,
        Night,
        Result
    };

    struct Layout {
        Rectangle garden{};
        Rectangle water{};
        Rectangle light{};
        Rectangle nextDay{};
        Rectangle help{};

        std::array<Rectangle, GreenGarden::BedCount> beds{};

        int width = 0;
        int height = 0;
        float scale = 1.0f;
    };

    GardenArt _art;
    std::array<std::size_t, GreenGarden::BedCount> _flowerColors{};

    GreenGarden _garden;
    Layout _layout;

    Tool _tool = Tool::None;
    Phase _phase = Phase::Theory;

    bool _theoryShown = false;

    float _nightTime = 0.0f;
    float _noticeTime = 0.0f;

    const char* _notice = "";

    void BuildLayout();

    void ShowTheory();
    void ShowRules();
    void StartGame();

    void UpdateGame();
    void ApplyTool(int index);

    void AdvanceDay();
    void FinishGame();

    void Notify(const char* message);

    void DrawGarden() const;
    void DrawPlant(Rectangle bounds, int frame, int bedIndex) const;

    static int PlantFrame(const GreenGarden::Bed& bed);
    static const char* BedHint(const GreenGarden::Bed& bed);

    Texture2D _plashka{};
};


#endif //NASA_GAME4_H