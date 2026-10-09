#pragma once

#include <memory>

class Texture;
class IRendererBackend;

class TextureLoader
{
public:
    explicit TextureLoader(
        IRendererBackend& rendererBackend);

    // EN: Loads one texture through the configured rendering backend.
    //
    // JP: �ݒ肳�ꂽ Rendering Backend ��ʂ��āA
    //     �w�肳�ꂽ Texture Resource ��ǂݍ��ށB
    std::unique_ptr<Texture> Load(
        const char* filePath);

private:
    IRendererBackend& m_rendererBackend;
};
