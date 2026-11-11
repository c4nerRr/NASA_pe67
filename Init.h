//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_INIT_H
#define NASA_INIT_H
#include "ProfilerTIME.h"


class Init {
    ProfilerTIME _profiler;


    public:
    Init();
    ~Init();

    void Run();
};


#endif //NASA_INIT_H