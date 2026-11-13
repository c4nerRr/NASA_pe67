//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_IGAME_H
#define NASA_IGAME_H

class IGame {

    protected:
    bool IsFinished = false;

    public:
    virtual ~IGame() = default;
    [[nodiscard]] bool GetFinished() const{ return IsFinished; }
};

#endif //NASA_IGAME_H