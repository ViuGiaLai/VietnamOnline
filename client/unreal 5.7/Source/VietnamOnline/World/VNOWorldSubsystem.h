#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "VNOWorldSubsystem.generated.h"

/** Zone bootstrap, Cesium/OSM hooks (doc 3.2, FR-06) */
UCLASS()
class VIETNAMONLINE_API UVNOWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintPure, Category = "VNO|World")
    FString GetCesiumToken() const { return CesiumToken; }

    UFUNCTION(BlueprintPure, Category = "VNO|World")
    bool IsCesiumReady() const { return bCesiumPluginEnabled; }

private:
    FString CesiumToken;
    FString OsmOverpassUrl;
    bool bCesiumPluginEnabled = false;
};
