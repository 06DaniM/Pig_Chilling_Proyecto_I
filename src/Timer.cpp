#include "Timer.h"

Timer::Timer(float seconds) {
    Start(seconds);
}

void Timer::Start(float seconds) {
    targetTime = seconds;
    currentTime = 0.0f;
    finished = false;
}

void Timer::Update(float deltaTime) {
    if (!finished) {
        currentTime += deltaTime;
        if (currentTime >= targetTime) {
            finished = true;
        }
    }
}

bool Timer::IsFinished() const {
    return finished;
}

bool Timer::IsRunning() const {
    return !finished;
}
