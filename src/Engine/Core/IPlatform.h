#pragma once

class IPlatform
{
public:
    virtual ~IPlatform() = default;

    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;

    virtual bool ProcessEvents() = 0;

    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
};