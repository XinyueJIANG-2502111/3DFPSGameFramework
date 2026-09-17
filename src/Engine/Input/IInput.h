#pragma once

#include "Engine/Input/InputTypes.h"

class IInput
{
public:
    virtual ~IInput() = default;

    virtual void Update() = 0;

    virtual bool IsKeyDown(KeyCode key) const = 0;
    virtual bool IsKeyPressed(KeyCode key) const = 0;
    virtual bool IsKeyReleased(KeyCode key) const = 0;

    virtual bool IsMouseButtonDown(MouseButton button) const = 0;
    virtual bool IsMouseButtonPressed(MouseButton button) const = 0;
    virtual bool IsMouseButtonReleased(MouseButton button) const = 0;

    virtual MouseDelta GetMouseDelta() const = 0;
};