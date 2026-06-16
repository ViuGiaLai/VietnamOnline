#include "World/VNOWorldSubsystem.h"
#include "Core/VNOGameInstance.h"

void UVNOWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        if (UVNOGameInstance* VGI = Cast<UVNOGameInstance>(GI))
        {
            if (UVNOEnvSettings* Env = VGI->GetEnv())
            {
                CesiumToken = Env->Get(TEXT("CESIUM_ION_TOKEN"), TEXT(""));
                OsmOverpassUrl = Env->Get(TEXT("OSM_OVERPASS_URL"), TEXT(""));
            }
        }
    }
#if WITH_EDITOR
    bCesiumPluginEnabled = false;
#else
    bCesiumPluginEnabled = FModuleManager::Get().IsModuleLoaded(TEXT("CesiumRuntime"));
#endif
    if (!CesiumToken.IsEmpty())
    {
        UE_LOG(LogTemp, Log, TEXT("[World] Cesium Ion token loaded — enable CesiumForUnreal plugin in .uproject"));
    }
    if (!OsmOverpassUrl.IsEmpty())
    {
        UE_LOG(LogTemp, Log, TEXT("[World] OSM Overpass: %s"), *OsmOverpassUrl);
    }
}
