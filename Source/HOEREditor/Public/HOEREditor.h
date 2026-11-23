#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * Main editor module for HOER, registering editor-time asset validators and extensions.
 */
class FHOEREditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
