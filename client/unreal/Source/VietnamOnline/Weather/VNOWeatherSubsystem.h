#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "VNOWeatherSubsystem.generated.h"

/** OpenWeatherMap sync (doc L.1 — 15 phut) */
UCLASS()
class VIETNAMONLINE_API UVNOWeatherSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override { return !IsTemplate(); }
    virtual TStatId GetStatId() const override;

    UFUNCTION(BlueprintPure, Category = "VNO|Weather")
    FString GetDisplayString() const;

    UFUNCTION(BlueprintPure, Category = "VNO|Weather")
    float GetTemperatureC() const { return TemperatureC; }

private:
    void FetchWeather();
    void OnWeatherResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);

    float TemperatureC = 32.f;
    FString WeatherLabel = TEXT("Nang dep");
    float SyncTimer = 0.f;
    float SyncIntervalSec = 900.f;
    FString ApiKey;
    float ZoneLat = 15.8794f;
    float ZoneLng = 108.3294f;
};
