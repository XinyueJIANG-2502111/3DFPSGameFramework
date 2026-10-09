#pragma once

#include "Engine/Math/Vector3.h"

class Texture;

enum class BillboardBlendMode
{
    Alpha,
    Additive
};

struct BillboardRenderData
{
    Vector3 position{};

    float width = 1.0f;
    float height = 1.0f;

    float rotationRadians = 0.0f;

    Vector3 color{
        1.0f,
        1.0f,
        1.0f
    };

    float alpha = 1.0f;
};

struct BillboardDrawSettings
{
    BillboardBlendMode blendMode =
        BillboardBlendMode::Alpha;

    // EN: Texture belongs to the whole billboard batch, not to each
    //     particle, so one draw call can bind it once for all quads.
    // JP: Texture �� Particle �ł͂Ȃ� Billboard Batch �S�̂ɐݒ肷��B
    //     �e Quad ���ƂɎ���킸�A�P�� Draw Call �Ō��L����B
    const Texture* texture = nullptr;
};
