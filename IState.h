
#ifndef NASA_ISTATE_H
#define NASA_ISTATE_H

class IState {
    public:
    virtual ~IState() = default;

    virtual void OnEnter() {};

    virtual void Update(float dt) = 0;
    virtual void Draw() const = 0;
};

#endif //NASA_ISTATE_H