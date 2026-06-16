#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/** Vietnam Online UE5 module: Multiplayer — see STRUCTURE.md */
class FVNOMultiplayerModule : public IModuleInterface {
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};