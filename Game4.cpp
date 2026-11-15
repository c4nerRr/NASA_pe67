#include "Game4.h"

#include "GameManager.h"
#include "InputManager.h"
#include "raylib.h"

namespace {
    constexpr float nightDuration = 1.0f;

    void Label(const char* text, float x, float y, int size, Color color) {
        DrawText(text, static_cast<int>(x), static_cast<int>(y), size, color);
    }

    void Button(Rectangle rect, const char* text, bool selected, float scale) {
        const int size = std::max(10, static_cast<int>(22 * scale));

        DrawRectangleRec(rect, selected ? Color{ 55, 105, 145, 255 } : Color{ 35, 45, 60, 255 });

        DrawRectangleLinesEx(rect, 2.0f, selected ? SKYBLUE : GRAY);

        Label(text, rect.x + (rect.width - MeasureText(text, size)) * 0.5f, rect.y + (rect.height - size) * 0.5f, size, RAYWHITE);
    }
}


Game4::Game4() = default;

Game4::~Game4()
{
    if (_plashka.id != 0) {
        UnloadTexture(_plashka);
    }
}

void Game4::OnEnter() {
    _art.Load();
    BuildLayout();

    if (_plashka.id == 0) {
        _plashka = LoadTexture("assets/game4/plashka.png");
    }

    if (IsFinished) {
        _phase = Phase::Result;

        GameManager::Instanse().ShowDialogue({
            "This greenhouse mission is already complete!"
            });

        return;
    }

    if (!_theoryShown) {
        _phase = Phase::Theory;
        ShowTheory();
    }
    else {
        _phase = Phase::Rules;
        ShowRules();
    }
}

void Game4::Update(float dt) {
    if (
        _layout.width != GetScreenWidth() ||
        _layout.height != GetScreenHeight()
        ) {
        BuildLayout();
    }

    dt = std::max(0.0f, dt);
    _noticeTime = std::max(0.0f, _noticeTime - dt);

    switch (_phase) {
    case Phase::Theory:
        _phase = Phase::Rules;
        ShowRules();
        break;

    case Phase::Rules:
        StartGame();
        break;

    case Phase::Playing:
        UpdateGame();
        break;

    case Phase::Night:
        _nightTime -= dt;

        if (_nightTime <= 0.0f) {
            AdvanceDay();
        }

        break;

    case Phase::Result:
        GameManager::Instanse().ChangeState(GameState::Hub);
        break;
    }
}

void Game4::Draw() const {
    GardenArt::DrawFill(_art.Background(), { 0.0f,0.0f,static_cast<float>(GetScreenWidth()),static_cast<float>(GetScreenHeight()) });

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{ 0, 0, 0, 150 });

    const auto w = static_cast<float>(GetScreenWidth());
    const auto h = static_cast<float>(GetScreenHeight());
    const float s = std::min(w / 1920.0f, h / 1080.0f);

    const float pw = 1760.0f * s;
    const float ph = 1000.0f * s;
    DrawTexturePro(
        _plashka,
        { 0.0f, 0.0f, static_cast<float>(_plashka.width), static_cast<float>(_plashka.height) },
        { (w - pw) * 0.5f, (h - ph) * 0.5f, pw, ph },
        { 0.0f, 0.0f },
        0.0f,
        WHITE
    );

    const int titleSize = std::max(14, static_cast<int>(36 * s));
    const int font = std::max(10, static_cast<int>(24 * s));

    constexpr const char* title = "MOON GREENHOUSE";

    Label(
        title,
        (w - MeasureText(title, titleSize)) * 0.5f,
        h * 0.05f,
        titleSize,
        BLACK
    );

    if (_phase == Phase::Theory || _phase == Phase::Rules) {
        return;
    }

    Label(TextFormat("Day %d/%d   Water left: %d/%d   Reputation: %d", _garden.Day(), GreenGarden::MaxDays, _garden.WaterLeft(), GreenGarden::WaterPerDay, _garden.Reputation()), w * 0.27f, h * 0.12f, font, BLACK);

    Button(_layout.water, "WATER", _tool == Tool::Water, s);
    Button(_layout.light, "LIGHT", _tool == Tool::Light, s);

    Button(_layout.help, "RULES", false, s);
    Button(_layout.nextDay, "END DAY", false, s);

    DrawGarden();

    if (_noticeTime > 0.0f) {
        Label(_notice, (w - MeasureText(_notice, font)) * 0.5f, h * 0.79f, font, ORANGE);
    }

    Label("SPACE - leave attempt", w * 0.10f, h * 0.94f, font, DARKGRAY);

    if (_phase == Phase::Night) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{ 5, 8, 25, 205 });

        constexpr const char* message =
            "NIGHT - checking the greenhouse...";

        Label(message, (w - MeasureText(message, font)) * 0.5f, h * 0.48f, font, RAYWHITE);
    }
}

int Game4::GetReputation() const {
    return IsFinished ? _garden.Reputation() : 0;
}

void Game4::BuildLayout() {
    _layout.width = GetScreenWidth();
    _layout.height = GetScreenHeight();

    const auto w = static_cast<float>(_layout.width);
    const auto h = static_cast<float>(_layout.height);

    _layout.scale =
        std::min(w / 1920.0f, h / 1080.0f);

    _layout.water = {
        w * 0.37f, h * 0.215f,
        w * 0.12f, h * 0.065f
    };

    _layout.light = {
        w * 0.51f, h * 0.215f,
        w * 0.12f, h * 0.065f
    };

    _layout.help = {
        w * 0.83f, h * 0.2225f,
        w * 0.07f, h * 0.05f
    };

    _layout.nextDay = {
        w * 0.39f, h * 0.80f,
        w * 0.22f, h * 0.07f
    };

    _layout.garden = {
        w * 0.10f, h * 0.29f,
        w * 0.80f, h * 0.48f
    };

    for (int i = 0; i < GreenGarden::BedCount; ++i) {
        _layout.beds[i] = {
            w * (0.12f + i * 0.26f),
            h * 0.31f,
            w * 0.24f,
            h * 0.44f
        };
    }
}

void Game4::ShowTheory() {
    _theoryShown = GameManager::Instanse().ShowDialogue({
       "SPACE GARDENER: Imagine helping astronauts grow a fresh meal! "
       "Our game puts you in charge of a pretend Moon greenhouse. "
       "The ideas behind it come from real NASA plant research.",

       "Astronauts on the International Space Station have grown crops "
       "such as lettuce and radishes. Scientists study these gardens "
       "to learn how crews could grow some food on longer journeys.",

       "Much of a space crew's food arrives in packages from Earth. "
       "A garden can add fresh vegetables to the menu. Caring for living "
       "plants can also make a spacecraft feel a little more like home.",

       "NASA has a small space garden called Veggie. Some plants grow "
       "in special fabric pillows filled with a growing material and "
       "fertilizer. These help support the roots and supply nutrients.",

       "A plant needs light to make its own food. Space gardens can use "
       "LED lamps. Red and blue light are useful for growing plants, "
       "so some space gardens glow pink or purple!",

       "Roots need water, but they also need oxygen. Flooding the area "
       "around the roots can leave too little air for them. More water "
       "is not always better: a healthy garden needs a balance.",

       "On the space station, water behaves differently in microgravity. "
       "Scientists test ways to bring water and air to the roots. "
       "The Moon has gravity too; a Moon garden is a different challenge.",

       "Some NASA plant chambers use sensors and cameras to watch the "
       "plants. Equipment controls conditions such as temperature and "
       "moisture. Astronauts and scientists check how the plants respond.",

       "Students on Earth have helped test plants for future space "
       "gardens. You can think like a scientist too: look at each plant, "
       "notice what it needs, and check what happened after your care.",

       "In our game, water portions, fast growth and the 50 percent "
       "overwatering risk are made-up rules. Real plants do not follow "
       "a coin toss. The real lesson is to provide enough water and "
       "light without flooding the roots."
        });
}


void Game4::ShowRules() {
    GameManager::Instanse().ShowDialogue({
        std::string(
            "YOUR MISSION: Grow all three beds. Each needs "
        ) +
        std::to_string(GreenGarden::GrowthNeeded) +
        " successful growing days. You have " +
        std::to_string(GreenGarden::MaxDays) +
        " days to finish. Ready plants need no more care.",

        std::string(
            "WATER SUPPLY: You receive "
        ) +
        std::to_string(GreenGarden::WaterPerDay) +
        " portions each morning for the whole greenhouse. Unused supply "
        "does not carry over. Water already stored in a bed is different: "
        "it can last until the next day.",

        "TOOLS: Click WATER, then click a bed to pour one portion. "
        "Click LIGHT, then a bed to switch its lamp on or off. "
        "Click a selected tool again to put it away.",

        "ONE PORTION: A bed with one portion and its lamp on will grow "
        "at the end of the day. That water is used up. "
        "Tomorrow the bed will need water again.",

        "TWO PORTIONS: Too much water is risky! At night there is a "
        "50 percent chance the plant dies from overwatering. If it "
        "survives, it keeps one portion for tomorrow and grows if its lamp is on.",

        "THREE OR MORE PORTIONS: The plant will die that night. "
        "You may pour at most three times into one bed in a day. "
        "Every pour uses the shared supply and cannot be taken back.",

        "CHECK STORED WATER: Yesterday's stored portion counts too! "
        "Adding one more makes two portions and creates the same "
        "50 percent risk. Check the bed before watering it.",

        "LIGHT AND DROUGHT: Without light, a plant does not grow. "
        "Without water, it also misses a growing day. Two days in a row "
        "without water will kill it. Lamps stay on until you switch them off.",

        "END DAY: Nothing changes while you are deciding. Click END DAY "
        "when ready, then wait for the short night transition. "
        "The game checks water, light and growth once that night.",

        "REPUTATION: If every unfinished bed grows that day, each unused "
        "water portion earns one point. Neglecting plants earns no points. "
        "Your total counts only if you win the attempt.",

        "WIN OR TRY AGAIN: Grow all three beds to win. If a plant dies "
        "or time runs out, read the message and return to the Hub. "
        "The greenhouse stays open for another try. Only a win closes it.",

        "HELP: Read the hints above each bed. Click RULES to read these "
        "instructions again. Press SPACE during the day to leave the "
        "attempt. Returning starts a fresh greenhouse."
        });
}

void Game4::StartGame() {
    _garden = GreenGarden{};

    static_assert(GreenGarden::BedCount <= GardenDesign::colors.size());

    std::array<std::size_t, GardenDesign::colors.size()> palette{};

    for (std::size_t i = 0; i < palette.size(); ++i) {
        palette[i] = i;
    }

    for (std::size_t i = 0; i < _flowerColors.size(); ++i) {
        const auto pick = static_cast<std::size_t>(
            GetRandomValue(static_cast<int>(i), static_cast<int>(palette.size()) - 1)
            );

        std::swap(palette[i], palette[pick]);
        _flowerColors[i] = palette[i];
    }

    _tool = Tool::None;
    _nightTime = 0.0f;
    _noticeTime = 0.0f;
    _notice = "";

    _phase = Phase::Playing;
}

void Game4::UpdateGame() {
    if (InputManager::PressSPACE()) {
        GameManager::Instanse().ChangeState(GameState::Hub);
        return;
    }

    if (!InputManager::PressLeftMouse()) {
        return;
    }

    const Vector2 mouse = InputManager::MousePosition();

    if (CheckCollisionPointRec(mouse, _layout.help)) {
        ShowRules();
        return;
    }

    if (CheckCollisionPointRec(mouse, _layout.nextDay)) {
        _phase = Phase::Night;
        _nightTime = nightDuration;
        _noticeTime = 0.0f;
        return;
    }

    if (CheckCollisionPointRec(mouse, _layout.water)) {
        _tool = _tool == Tool::Water ? Tool::None : Tool::Water;
        return;
    }

    if (CheckCollisionPointRec(mouse, _layout.light)) {
        _tool = _tool == Tool::Light ? Tool::None : Tool::Light;
        return;
    }

    for (int i = 0; i < GreenGarden::BedCount; ++i) {
        if (CheckCollisionPointRec(mouse, _layout.beds[i])) {
            if (_tool == Tool::None) {
                Notify("Choose WATER or LIGHT first.");
            }
            else {
                ApplyTool(i);
            }

            return;
        }
    }
}

void Game4::ApplyTool(int index) {
    const auto& bed = _garden.GetBed(index);

    if (bed.Ready()) {
        Notify("This bed is ready. Care for the other plants!");
        return;
    }

    if (_tool == Tool::Light) {
        _garden.ToggleLight(index);
    }
    else if (_tool == Tool::Water) {
        if (!_garden.Water(index)) {
            Notify(_garden.WaterLeft() == 0 ? "No water left today." : "Three pours is the daily limit for one bed.");
        }
        else if (bed.Moisture() >= 3) {
            Notify("Flooded! This plant will not survive the night.");
        }
        else if (bed.Moisture() == 2) {
            Notify("Warning: this bed now has a 50 percent risk of dying.");
        }
    }
}

void Game4::AdvanceDay() {
    std::array<bool, GreenGarden::BedCount> rot{};

    for (int i = 0; i < GreenGarden::BedCount; ++i) {
        const auto& bed = _garden.GetBed(i);

        if (
            !bed.Ready() &&
            !bed.dead &&
            bed.Moisture() == 2
            ) {
            rot[i] = GetRandomValue(0, 1) == 0;
        }
    }

    _garden.AdvanceDay(rot);

    if (_garden.Result() == GreenGarden::Outcome::Playing) {
        _phase = Phase::Playing;

        Notify("A new day! Check water and light for every growing bed.");
    }
    else {
        FinishGame();
    }
}


void Game4::FinishGame() {
    _phase = Phase::Result;

    auto& manager = GameManager::Instanse();
    using Outcome = GreenGarden::Outcome;

    if (_garden.Result() == Outcome::Won) {
        IsFinished = true;

        manager.ShowDialogue({
            "MISSION COMPLETE! All three beds are ready. "
            "You grew food for the astronauts!",

            std::string("Reputation earned: ") +
            std::to_string(_garden.Reputation()) +
            ". Close this message to return to the Hub."
            });

        return;
    }

    std::string reason;

    if (_garden.Result() == Outcome::TimeUp) {
        reason =
            "Time ran out before all three beds were ready. "
            "Check both water and light each day.";
    }
    else {
        reason =
            "Bed " + std::to_string(_garden.FailedBed() + 1);

        reason += _garden.Result() == Outcome::Overwatered ? " died from overwatering. ""One total portion per day is the safe choice." : " dried out after two days without water. ""Watch the NEED WATER hint.";
    }

    manager.ShowDialogue({
        "TRY AGAIN! " + reason,

        "Close this message to return to the Hub. "
        "Enter the greenhouse again for a fresh attempt."
        });
}

void Game4::Notify(const char* message) {
    _notice = message;
    _noticeTime = 3.0f;
}

void Game4::DrawGarden() const {
    const float s = _layout.scale;

    const int font = std::max(10, static_cast<int>(22 * s));


    for (int i = 0; i < GreenGarden::BedCount; ++i) {
        const auto& bed = _garden.GetBed(i);
        const Rectangle r = _layout.beds[i];
        GardenArt::DrawFill(_art.Soil(), r);

        const float x = r.x + 12 * s;

        DrawRectangleLinesEx(r, 2.0f, bed.lightOn ? GOLD : DARKGREEN);

        Label(TextFormat("BED %d", i + 1), x, r.y + 12 * s, font, RAYWHITE);

        if (!bed.dead && !bed.Ready()) {
            const float iconSize = 32.0f * s;
            const float gap = 12.0f * s;

            const Rectangle lightIcon{ r.x + r.width - gap - iconSize,r.y + gap,iconSize,iconSize };

            Rectangle waterIcon = lightIcon;
            waterIcon.x -= iconSize + gap;

            GardenArt::DrawFit(_art.Water(bed.Moisture() > 0), waterIcon);

            GardenArt::DrawFit(_art.Light(bed.lightOn), lightIcon);
        }

        const char* hint = BedHint(bed);

        Label(hint, r.x + r.width - MeasureText(hint, font) - 12 * s,
            r.y + 48 * s,
            font,
            bed.dead || bed.Moisture() >= 2 ? ORANGE : SKYBLUE
        );

        const Rectangle plant{
            x,
            r.y + r.height * 0.26f,
            r.width - 24 * s,
            r.height * 0.35f
        };

        DrawPlant(plant, PlantFrame(bed), i);

        Label(TextFormat("Growth: %d/%d", bed.growth, GreenGarden::GrowthNeeded), x, r.y + r.height * 0.65f, font, RAYWHITE);

        if (!bed.Ready() && !bed.dead) {
            Label(TextFormat("Water: %d  (stored: %d)", bed.Moisture(), bed.storedWater ? 1 : 0), x, r.y + r.height * 0.73f, font, SKYBLUE);

            Label(bed.lightOn ? "Light: ON" : "Light: OFF", x, r.y + r.height * 0.81f, font, bed.lightOn ? GOLD : LIGHTGRAY);

            if (bed.dryDays > 0) {
                Label("Dry yesterday - water today!", x, r.y + r.height * 0.89f, font, ORANGE);
            }
        }
    }
}

void Game4::DrawPlant(Rectangle bounds, int frame, int bedIndex) const {
    GardenArt::DrawFit(_art.Flower(_flowerColors[bedIndex], frame), bounds);
}


int Game4::PlantFrame(const GreenGarden::Bed& bed) {
    if (bed.dead) {
        return 3;
    }

    if (bed.Ready()) {
        return 2;
    }

    return bed.growth == 0 ? 0 : 1;

}

const char* Game4::BedHint(const GreenGarden::Bed& bed) {
    if (bed.dead) {
        return "PLANT LOST";
    }

    if (bed.Ready()) {
        return "READY!";
    }

    if (bed.Moisture() >= 3) {
        return "! FATAL FLOOD";
    }

    if (bed.Moisture() == 2) {
        return "! 50% ROT RISK";
    }

    if (bed.Moisture() == 0 && !bed.lightOn) {
        return "! WATER + LIGHT";
    }

    if (bed.Moisture() == 0) {
        return "! NEED WATER";
    }

    if (!bed.lightOn) {
        return "! NEED LIGHT";
    }

    return "CARE READY";
}
