#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "vno/enums/WorldEnums.hpp"
#include "Interfaces/IHttpRequest.h"
#include "VNOWeatherSubsystem.generated.h"

/** Enhanced FR-11: Dynamic weather from OpenWeatherMap + simulated fallback + gameplay modifiers */
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

    /** Display string for HUD, e.g. "Nang dep 32C" */
    UFUNCTION(BlueprintPure, Category = "VNO|Weather")
    FString GetDisplayString() const;

    /** Get temperature in Celsius */
    UFUNCTION(BlueprintPure, Category = "VNO|Weather")
    float GetTemperatureC() const { return TemperatureC; }

    /** Get mapped weather type for gameplay effects (C++ only — vno::WeatherType is not a UENUM) */
    vno::WeatherType GetWeatherType() const { return CurrentType; }

    /** 0-1 normalized rain intensity for visual effects */
    UFUNCTION(BlueprintPure, Category = "VNO|Weather")
    float GetRainIntensity() const { return RainIntensity; }

    /** 0-1 fog density for visual effects */
    UFUNCTION(BlueprintPure, Category = "VNO|Weather")
    float GetFogDensity() const { return FogDensity; }

    /** Stamina regen multiplier (rain=slower, hot=slower, clear=faster) */
    UFUNCTION(BlueprintPure, Category = "VNO|Weather")
    float GetStaminaRegenModifier() const { return StaminaRegenMod; }

    /** Photo score bonus (storm/fog=atmospheric bonus, clear=neutral) */
    UFUNCTION(BlueprintPure, Category = "VNO|Weather")
    float GetPhotoScoreBonus() const { return PhotoScoreBonus; }

    /** Visibility range in cm (fog reduces this) */
    UFUNCTION(BlueprintPure, Category = "VNO|Weather")
    float GetVisibilityRangeCm() const { return VisibilityRangeCm; }

    /** Current wind speed in km/h */
    UFUNCTION(BlueprintPure, Category = "VNO|Weather")
    float GetWindSpeedKmh() const { return WindSpeedKmh; }

    /** Time of day: 0-24 (simulated when no API) */
    UFUNCTION(BlueprintPure, Category = "VNO|Weather")
    float GetHourOfDay() const { return HourOfDay; }

    /** Is it night time? (20h-5h) */
    UFUNCTION(BlueprintPure, Category = "VNO|Weather")
    bool IsNight() const { return HourOfDay < 5.f || HourOfDay >= 20.f; }

    /** Description of weather effects for tooltips */
    UFUNCTION(BlueprintPure, Category = "VNO|Weather")
    FString GetEffectsDescription() const;

private:
    void FetchWeather();
    void OnWeatherResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
    void UpdateSimulatedWeather(float DeltaTime);
    void ApplyWeatherTransition(vno::WeatherType Target, float Temp, float Wind);
    void UpdateGameplayModifiers();

    // Real weather data
    float TemperatureC = 32.f;
    FString WeatherLabel = TEXT("Nang dep");
    vno::WeatherType CurrentType = vno::WeatherType::SUNNY_CLEAR;
    vno::WeatherType TargetType = vno::WeatherType::SUNNY_CLEAR;
    float WeatherTransitionAlpha = 1.f;

    // Simulated weather
    float SimulatedTimer = 0.f;
    float SimulatedChangeInterval = 120.f; // change every ~2 min when simulating

    // Visual effects
    float RainIntensity = 0.f;
    float FogDensity = 0.f;
    float VisibilityRangeCm = 999900.f;

    // Gameplay modifiers
    float StaminaRegenMod = 1.0f;
    float PhotoScoreBonus = 0.f;

    // Wind
    float WindSpeedKmh = 5.f;

    // Time of day
    float HourOfDay = 12.f; // noon default
    float DaySpeedMultiplier = 1.f; // 1 = real-time, higher = faster day cycle

    // Performance: throttle weather updates to ~4Hz
    int32 ThrottleCounter = 15; // start high so first Tick() runs immediately
    static constexpr int32 THROTTLE_SKIP = 15; // skip 15 frames (~4Hz at 60fps)

    // API sync
    float SyncTimer = 0.f;
    float SyncIntervalSec = 900.f;
    FString ApiKey;
    float ZoneLat = 15.8794f;
    float ZoneLng = 108.3294f;
    bool bFirstSyncDone = false;
    bool bApiConfigured = false;
};
