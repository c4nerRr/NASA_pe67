
#ifndef NASA_GREENGARDEN_H
#define NASA_GREENGARDEN_H
#include <array>


class GreenGarden {
public:

    GreenGarden();
    ~GreenGarden();

    static constexpr int BedCount = 3;
    static constexpr int WaterPerDay = 4;
    static constexpr int GrowthNeeded = 3;
    static constexpr int MaxDays = 4;
    static constexpr int MaxPours = 3;

    enum class Outcome {
        Playing,
        Won,
        Overwatered,
        DriedOut,
        TimeUp
    };

    struct Bed {
        int growth = 0;
        int pours = 0;
        int dryDays = 0;

        bool storedWater = false;
        bool lightOn = false;
        bool dead = false;

        [[nodiscard]] int Moisture() const;
        [[nodiscard]] bool Ready() const;
    };
    [[nodiscard]] const Bed& GetBed(int index) const;
    [[nodiscard]] int Day() const;
    [[nodiscard]] int WaterLeft() const;
    [[nodiscard]] int Reputation() const;
    [[nodiscard]] int FailedBed() const;

    [[nodiscard]] Outcome Result() const;
    bool Water(int index);
    void ToggleLight(int index);
    void AdvanceDay(const std::array<bool, BedCount>& rot);

    private:

    std::array<Bed, BedCount> _beds{};

    int _day = 1;
    int _water = WaterPerDay;
    int _reputation = 0;
    int _failedBed = -1;

    Outcome _outcome = Outcome::Playing;

    [[nodiscard]] bool CanCareFor(int index) const;

};


#endif //NASA_GREENGARDEN_H