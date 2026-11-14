
#ifndef NASA_INIT_H
#define NASA_INIT_H
#include "GameAssets.h"
#include "HubConfig.h"
#include "ProfilerTIME.h"


class Init {
    ProfilerTIME _profiler;
    GameAssets _assets;
    const HubConfig _config;

    public:
    Init();
    ~Init();

    void Run();
};


#endif //NASA_INIT_H