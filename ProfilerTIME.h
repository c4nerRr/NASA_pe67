

#ifndef NASA_PROFILERTIME_H
#define NASA_PROFILERTIME_H
#include <vector>
#include <string>

class ProfilerTIME {
private:
    struct Zapis {
        const char* _name{};
        float _time{};
        int lvl{};
    };
    std::vector<Zapis> _zapis{};
    std::vector<Zapis> _last_zapis{};
    int _currLVL{};

public:
    ProfilerTIME();
    ~ProfilerTIME();

    void BeginFrame();
    void AddZapis(const char* name, float time );
    void DrawZapis() const;

    class ScopeMarker {
    private:
        long long _start{}; //LARGE_INTEGER
        ProfilerTIME &profiler;
        const char* _name{};

    public:
        ScopeMarker(const char* name, ProfilerTIME& profiler);
        ~ScopeMarker();
    };
};


#endif //NASA_PROFILERTIME_H