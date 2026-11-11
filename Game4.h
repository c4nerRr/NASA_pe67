//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_GAME4_H
#define NASA_GAME4_H
#include "IGame.h"
#include "IState.h"


class Game4 : public IGame, public IState {

    public:
    Game4();
    ~Game4() override;

    void Start();
    void Update();
    void End();
};


#endif //NASA_GAME4_H