#include "Engine/Core/Time.h"

#include <chrono>

namespace
{
    using Clock = std::chrono::steady_clock;

    Clock::time_point g_previousTime;
}

float Time::s_deltaTime = 0.0f;
float Time::s_totalTime = 0.0f;

void Time::Initialize()
{
    g_previousTime = Clock::now();

    s_deltaTime = 0.0f;
    s_totalTime = 0.0f;
}

void Time::Update()
{
    const auto currentTime = Clock::now();

    const std::chrono::duration<float> elapsed =
        currentTime - g_previousTime;

    g_previousTime = currentTime;

    s_deltaTime = elapsed.count();
    s_totalTime += s_deltaTime;
}

float Time::DeltaTime()
{
    return s_deltaTime;
}

float Time::TotalTime()
{
    return s_totalTime;
}