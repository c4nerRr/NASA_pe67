//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_GAME1_H
#define NASA_GAME1_H
#include "IGame.h"
#include "IState.h"


class Game1 : public IGame, public IState {

    public:
    Game1();
    ~Game1() override;

    void Update(float dt) override;
    void Draw() const override;
};


#endif //NASA_GAME1_H