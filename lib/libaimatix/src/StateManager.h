#pragma once

class IState {
public:
    virtual ~IState() {}
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    virtual void onDraw() = 0;
    virtual void onButtonA() = 0;
    virtual void onButtonB() = 0;
    virtual void onButtonC() = 0;
    virtual void onButtonALongPress() = 0;
    virtual void onButtonCLongPress() = 0;
};

class StateManager {
public:
    StateManager();
    void setState(IState* state);
    IState* getCurrentState() const;
    void handleButtonA();
    void handleButtonB();
    void handleButtonC();
    void handleButtonALongPress();
    void handleButtonCLongPress();
private:
    IState* currentState;
}; 