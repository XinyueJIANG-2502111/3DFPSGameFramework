#pragma once

class Time
{
public:
    static void Initialize();
    static void Update();

    static float DeltaTime();
    static float TotalTime();

private:
    static float s_deltaTime;
    static float s_totalTime;
};