#pragma once

class Texture;
class ITextureResource;

class TextureResourceAccess
{
public:
    // EN: Gives rendering backends controlled access to a Texture's
    //     backend-owned resource without exposing native handles to Game.
    //
    // JP: Game �� Native Handle ���o�����ɁA
    //     Rendering Backend �݂̂� Texture Resource �֎������A�N�Z�X����B
    static const ITextureResource* Get(
        const Texture& texture);
};
