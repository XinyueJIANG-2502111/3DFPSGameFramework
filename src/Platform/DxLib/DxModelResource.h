#pragma once

#include "Engine/Rendering/IModelResource.h"
#include "Platform/DxLib/DxModel.h"

class DxModelResource final : public IModelResource
{
public:
    explicit DxModelResource(
        const char* filePath);

    bool IsValid() const;

    // Temporary backend access.
    // This will only be used inside the DxLib rendering layer.
    DxModel& GetModel();
    const DxModel& GetModel() const;

private:
    // EN: Owns the DxLib-specific native model resource through RAII.
    //
    // JP: RAII ‚ğ’Ê‚µ‚Ä DxLib ŒÅ—L‚Ì Native Model Resource ‚ğŠ—L‚·‚éB
    DxModel m_model;
};