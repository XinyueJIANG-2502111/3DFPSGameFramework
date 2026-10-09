#include "Engine/Effects/Screen/LowHealthScreenEffect.h"

#include "Engine/Rendering/Renderer.h"

#include <algorithm>
#include <cmath>

void LowHealthScreenEffect::SetHealthRatio(float ratio)
{
    // EN: Invalid health input is treated as healthy so a bad gameplay value
    //     cannot accidentally cover the entire screen red.
    // JP: Invalid health input falls back to the healthy state.
    if (!std::isfinite(ratio))
    {
        m_healthRatio = 1.0f;
        return;
    }

    m_healthRatio =
        std::clamp(
            ratio,
            0.0f,
            1.0f);
}

void LowHealthScreenEffect::Update(float deltaTime)
{
    if (!std::isfinite(deltaTime) ||
        deltaTime <= 0.0f)
    {
        return;
    }

    m_elapsedTime += deltaTime;
}

float LowHealthScreenEffect::CalculateIntensity() const
{
    constexpr float threshold =
        0.30f;

    if (m_healthRatio >= threshold)
    {
        return 0.0f;
    }

    // EN: Convert low health into a normalized severity value.
    //
    // JP: Low Health 状態を正規化された Severity に変換する。
    const float severity =
        std::clamp(
            (threshold - m_healthRatio) /
            threshold,
            0.0f,
            1.0f);

    // EN: Lower health produces a slightly faster pulse.
    //
    // JP: HP が低いほど少し速い Pulse を生成する。
    const float frequencyHz =
        0.9f +
        0.5f * severity;

    const float phase =
        std::fmod(
            m_elapsedTime * frequencyHz,
            1.0f);

    const auto smoothstep =
        [](float start,
            float end,
            float value)
        {
            const float t =
                std::clamp(
                    (value - start) /
                    (end - start),
                    0.0f,
                    1.0f);

            return t * t * (3.0f - 2.0f * t);
        };

    // EN: Fast rise, short hold, smooth fade, then rest.
    //
    // JP: 素早く明るくなり、少し維持し、滑らかに暗くなってから休止する。
    const float fadeIn =
        smoothstep(
            0.00f,
            0.10f,
            phase);

    const float fadeOut =
        1.0f -
        smoothstep(
            0.48f,
            0.60f,
            phase);

    const float pulse =
        fadeIn * fadeOut;

    const float minimumIntensity =
        0.02f * severity;

    const float maximumIntensity =
        std::clamp(
            severity * 1.3f,
            0.0f,
            1.0f);

    const float intensity =
        minimumIntensity +
        (maximumIntensity - minimumIntensity) *
        pulse;

    return std::clamp(
        intensity,
        0.0f,
        1.0f);
}

//float LowHealthScreenEffect::CalculateIntensity() const
//{
//    constexpr float threshold = 0.30f;
//
//    if (m_healthRatio >= threshold)
//    {
//        return 0.0f;
//    }
//
//    const float severity =
//        std::clamp(
//            (threshold - m_healthRatio) /
//                threshold,
//            0.0f,
//            1.0f);
//
//    // EN: Treat the pulse rate explicitly as cycles per second.
////     std::sin() expects radians, so frequency in Hz must be
////     converted to angular frequency with 2 * PI.
////
//// JP: Pulse Rate は明示的に 1 秒あたりの Cycle 数として扱う。
////     std::sin() は Radian を受け取るため、Hz から
////     2 * PI を使って Angular Frequency に変換する。
//    constexpr float TwoPi =
//        6.28318530717958647692f;
//
//
//    // EN: Lower health produces a faster pulse.
//    //     V1 ranges from roughly 60 BPM to 96 BPM.
//    //
//    // JP: HP が低いほど Pulse を速くする。
//    //     V1 ではおよそ 60 BPM ～ 96 BPM の範囲とする。
//    const float pulseFrequencyHz =
//        1.0f +
//        0.6f * severity;
//
//
//    const float pulsePhase =
//        m_elapsedTime *
//        pulseFrequencyHz *
//        TwoPi;
//
//
//    // EN: Convert the sine wave into the [0, 1] range.
//    //
//    // JP: Sine Wave を [0, 1] Range に変換する。
//    const float sinePulse =
//        0.5f +
//        0.5f *
//        std::sin(
//            pulsePhase);
//
//
//    // EN: Square the waveform so the bright part becomes shorter
//    //     and more pronounced instead of behaving like a slow,
//    //     uniform breathing animation.
//    //
//    // JP: Waveform を二乗し、ゆっくり均一な呼吸表現ではなく、
//    //     明るい瞬間が短く強く感じられる Pulse にする。
//    const float shapedPulse =
//        sinePulse *
//        sinePulse;
//
//
//    // EN: Keep only a small baseline glow between pulses.
//    //     The large difference between the minimum and maximum
//    //     makes the low-health rhythm easier to perceive.
//    //
//    // JP: Pulse 間には小さな Baseline Glow だけを残す。
//    //     Minimum と Maximum の差を大きくすることで、
//    //     Low Health の Rhythm を視認しやすくする。
//    constexpr float minimumPulse =
//        0.08f;
//
//    const float pulse =
//        minimumPulse +
//        (1.0f - minimumPulse) *
//        shapedPulse;
//
//
//    const float intensity =
//        severity *
//        pulse;
//
//    return std::clamp(
//        intensity,
//        0.0f,
//        1.0f);
//}

void LowHealthScreenEffect::Render(Renderer& renderer) const
{
    const float intensity =
        CalculateIntensity();

    if (intensity <= 0.0f)
    {
        return;
    }

    // EN: Renderer owns the backend pass; this effect only supplies its
    //     current intensity and never participates in world VFX ownership.
    // JP: Renderer owns the backend pass; this class supplies only intensity.
    renderer.DrawLowHealthOverlay(
        LowHealthScreenEffectData{
            intensity});
}
