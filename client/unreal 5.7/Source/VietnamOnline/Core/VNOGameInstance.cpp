#include "Core/VNOGameInstance.h"
#include "World/VNOWorldSubsystem.h"
#include "Weather/VNOWeatherSubsystem.h"

void UVNOGameInstance::Init()
{
    Super::Init();
    EnvSettings = NewObject<UVNOEnvSettings>(this);
    if (EnvSettings->LoadEnvFile())
    {
        ZoneId = EnvSettings->Get(TEXT("VNO_DEFAULT_ZONE"), TEXT("HOI_AN"));
    }
    UE_LOG(LogTemp, Log, TEXT("[VNO] GameInstance init — zone %s"), *ZoneId);
}
