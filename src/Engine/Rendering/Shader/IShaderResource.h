#pragma once

class IShaderResource
{
public:
    virtual ~IShaderResource() = default;

    IShaderResource(const IShaderResource&) = delete;
    IShaderResource& operator=(const IShaderResource&) = delete;

protected:
    // EN: Backend shader resources are created by the active
    //     rendering backend and owned through this interface.
    //
    // JP: Backend Shader Resource ‚ÍŒ»İ‚Ì Rendering Backend ‚É‚æ‚Á‚Ä
    //     ¶¬‚³‚êA‚±‚Ì Interface ‚ğ’Ê‚µ‚ÄŠ—L‚³‚ê‚éB
    IShaderResource() = default;
};