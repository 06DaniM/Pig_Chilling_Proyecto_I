#ifndef TIMER_H
#define TIMER_H

class Timer {
private:
    float targetTime;
    float currentTime;
    bool finished;

public:
    Timer(float seconds = 0.0f);

    void Start(float seconds);      // Reinicia el temporizador
    void Update(float deltaTime);   // Llamar cada frame
    bool IsFinished() const;        // Saber si terminó
    bool IsRunning() const;         // Saber si está en curso
};

#endif