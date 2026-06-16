#include "Weather/VNOWeatherSubsystem.h"
#include "Core/VNOGameInstance.h"
#include "Core/VNOEnvSettings.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Math/UnrealMathUtility.h"

void UVNOWeatherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    if (UVNOGameInstance* GI = Cast<UVNOGameInstance>(GetWorld()->GetGameInstance()))
    {
        if (UVNOEnvSettings* Env = GI->GetEnv())
        {
            ApiKey = Env->Get(TEXT("OPENWEATHERMAP_API_KEY"), TEXT(""));
            SyncIntervalSec = Env->GetFloat(TEXT("WEATHER_SYNC_INTERVAL_MIN"), 15.f) * 60.f;
            ZoneLat = Env->GetFloat(TEXT("VNO_ZONE_HOI_AN_LAT"), 15.8794f);
            ZoneLng = Env->GetFloat(TEXT("VNO_ZONE_HOI_AN_LNG"), 108.3294f);
            DaySpeedMultiplier = Env->GetFloat(TEXT("VNO_DAY_SPEED"), 1.f);
        }
    }
    bApiConfigured = !ApiKey.IsEmpty();
    if (bApiConfigured)
    {
        FetchWeather();
        UE_LOG(LogTemp, Log, TEXT("[FR-11] Weather: OpenWeatherMap API configured — Hoi An %.4f, %.4f"),
            ZoneLat, ZoneLng);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[FR-11] Weather: No API key — simulating weather dynamics"));
        // Start with random simulated weather
        TargetType = static_cast<vno::WeatherType>(FMath::RandRange(0, static_cast<int32>(vno::WeatherType::CLOUDY)));
        ApplyWeatherTransition(TargetType, 30.f, 5.f);
    }
    // Start at real time of day
    FDateTime Now = FDateTime::Now();
    HourOfDay = static_cast<float>(Now.GetHour()) + static_cast<float>(Now.GetMinute()) / 60.f;
}

void UVNOWeatherSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UVNOWeatherSubsystem::Tick(float DeltaTime)
{
    // Performance: throttle weather updates to ~4Hz
    ThrottleCounter++;
    if (ThrottleCounter < THROTTLE_SKIP) return;
    ThrottleCounter = 0;

    // Advance time of day (1 real second = DaySpeedMultiplier game seconds)
    HourOfDay = FMath::Fmod(HourOfDay + DeltaTime * THROTTLE_SKIP * DaySpeedMultiplier / 3600.f, 24.f);

    // Smooth weather transition
    if (WeatherTransitionAlpha < 1.f)
    {
        float AdjustedDelta = DeltaTime * THROTTLE_SKIP;
        WeatherTransitionAlpha = FMath::Min(1.f, WeatherTransitionAlpha + AdjustedDelta * 0.05f);
        float T = FMath::InterpEaseInOut(0.f, 1.f, WeatherTransitionAlpha, 2.f);
        RainIntensity = FMath::Lerp(RainIntensity,
            (TargetType == vno::WeatherType::RAIN_LIGHT) ? 0.3f :
            (TargetType == vno::WeatherType::RAIN_HEAVY || TargetType == vno::WeatherType::STORM) ? 0.8f : 0.f, T);
        FogDensity = FMath::Lerp(FogDensity,
            (TargetType == vno::WeatherType::FOG) ? 0.7f : 0.f, T);
        VisibilityRangeCm = FMath::Lerp(VisibilityRangeCm,
            (TargetType == vno::WeatherType::FOG) ? 50000.f : 999900.f, T);
    }

    // Update gameplay modifiers
    UpdateGameplayModifiers();

    // API sync timer
    float AdjustedDelta = DeltaTime * THROTTLE_SKIP;
    SyncTimer += AdjustedDelta;
    if (bApiConfigured && SyncTimer >= SyncIntervalSec)
    {
        SyncTimer = 0.f;
        FetchWeather();
    }

    // Simulated weather change (when no API)
    if (!bApiConfigured)
    {
        UpdateSimulatedWeather(AdjustedDelta);
    }
}

void UVNOWeatherSubsystem::UpdateSimulatedWeather(float DeltaTime)
{
    SimulatedTimer += DeltaTime;
    if (SimulatedTimer >= SimulatedChangeInterval)
    {
        SimulatedTimer = 0.f;

        // Weather probability influenced by time of day
        TArray<vno::WeatherType> Candidates;

        // Daytime (5h-18h): mostly clear/sunny
        if (HourOfDay >= 5.f && HourOfDay <= 18.f)
        {
            Candidates.Add(vno::WeatherType::SUNNY_CLEAR);
            Candidates.Add(vno::WeatherType::SUNNY_CLEAR);
            Candidates.Add(vno::WeatherType::CLOUDY);
            if (HourOfDay >= 12.f && HourOfDay <= 15.f)
                Candidates.Add(vno::WeatherType::SUNNY_HOT);
            if (HourOfDay >= 6.f && HourOfDay <= 8.f)
                Candidates.Add(vno::WeatherType::FOG);
        }
        // Night (18h-5h): cooler, possible rain/fog
        else
        {
            Candidates.Add(vno::WeatherType::CLOUDY);
            Candidates.Add(vno::WeatherType::CLOUDY);
            Candidates.Add(vno::WeatherType::FOG);
            Candidates.Add(vno::WeatherType::RAIN_LIGHT);
        }

        // 10% chance of rain any time (tropical climate)
        if (FMath::FRand() < 0.1f)
        {
            Candidates.Add(vno::WeatherType::RAIN_LIGHT);
            if (FMath::FRand() < 0.3f)
                Candidates.Add(vno::WeatherType::RAIN_HEAVY);
        }

        vno::WeatherType NewType = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
        if (NewType != CurrentType)
        {
            TargetType = NewType;
            WeatherTransitionAlpha = 0.f;
            CurrentType = NewType;

            float Temp = 30.f;
            if (NewType == vno::WeatherType::SUNNY_HOT) Temp = 36.f;
            else if (NewType == vno::WeatherType::RAIN_LIGHT || NewType == vno::WeatherType::RAIN_HEAVY) Temp = 26.f;
            else if (NewType == vno::WeatherType::STORM) Temp = 24.f;
            else if (NewType == vno::WeatherType::FOG) Temp = 28.f;
            else if (NewType == vno::WeatherType::CLOUDY) Temp = 30.f;
            TemperatureC = Temp;

            // Night cooling
            if (IsNight()) TemperatureC -= 4.f;

            WindSpeedKmh = (NewType == vno::WeatherType::STORM) ? 50.f :
                          (NewType == vno::WeatherType::RAIN_HEAVY) ? 30.f :
                          (NewType == vno::WeatherType::RAIN_LIGHT) ? 15.f : 5.f;

            WeatherLabel = UTF8_TO_TCHAR(vno::weatherName(NewType));
        }
    }
}

void UVNOWeatherSubsystem::ApplyWeatherTransition(vno::WeatherType Target, float Temp, float Wind)
{
    CurrentType = Target;
    TargetType = Target;
    TemperatureC = Temp;
    WindSpeedKmh = Wind;
    WeatherTransitionAlpha = 0.f;

    RainIntensity = (Target == vno::WeatherType::RAIN_LIGHT) ? 0.3f :
                   (Target == vno::WeatherType::RAIN_HEAVY || Target == vno::WeatherType::STORM) ? 0.8f : 0.f;
    FogDensity = (Target == vno::WeatherType::FOG) ? 0.7f : 0.f;
    VisibilityRangeCm = (Target == vno::WeatherType::FOG) ? 50000.f : 999900.f;
}

void UVNOWeatherSubsystem::UpdateGameplayModifiers()
{
    // Stamina regen: hot and rainy reduce regen, clear/normal is best
    switch (CurrentType)
    {
    case vno::WeatherType::SUNNY_HOT:
        StaminaRegenMod = 0.7f; break; // hot = slower regen
    case vno::WeatherType::RAIN_HEAVY:
    case vno::WeatherType::STORM:
        StaminaRegenMod = 0.6f; break; // storm = hard to rest
    case vno::WeatherType::RAIN_LIGHT:
        StaminaRegenMod = 0.85f; break;
    case vno::WeatherType::FOG:
        StaminaRegenMod = 0.9f; break;
    default:
        StaminaRegenMod = 1.0f; break; // clear/cloudy = normal
    }

    // Night penalty
    if (IsNight())
        StaminaRegenMod *= 0.85f;

    // Photo score bonus: dramatic weather = better photos
    switch (CurrentType)
    {
    case vno::WeatherType::STORM:
        PhotoScoreBonus = 0.5f; break;
    case vno::WeatherType::RAIN_HEAVY:
        PhotoScoreBonus = 0.3f; break;
    case vno::WeatherType::FOG:
        PhotoScoreBonus = 0.2f; break;
    case vno::WeatherType::RAIN_LIGHT:
        PhotoScoreBonus = 0.1f; break;
    case vno::WeatherType::SUNNY_HOT:
        PhotoScoreBonus = -0.1f; break; // harsh light
    default:
        PhotoScoreBonus = 0.f; break;
    }
}

TStatId UVNOWeatherSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UVNOWeatherSubsystem, STATGROUP_Tickables);
}

FString UVNOWeatherSubsystem::GetDisplayString() const
{
    FString Icon;
    switch (CurrentType)
    {
    case vno::WeatherType::SUNNY_CLEAR: Icon = TEXT("(O)"); break;
    case vno::WeatherType::SUNNY_HOT:   Icon = TEXT("(H)"); break;
    case vno::WeatherType::CLOUDY:      Icon = TEXT("(~)"); break;
    case vno::WeatherType::RAIN_LIGHT:  Icon = TEXT("(/)"); break;
    case vno::WeatherType::RAIN_HEAVY:  Icon = TEXT("(/)"); break;
    case vno::WeatherType::STORM:       Icon = TEXT("(!)"); break;
    case vno::WeatherType::FOG:         Icon = TEXT("(=)"); break;
    default: Icon = TEXT(""); break;
    }

    FString TimeStr;
    int32 H = FMath::FloorToInt(HourOfDay);
    int32 M = static_cast<int32>(FMath::Fmod(HourOfDay * 60.f, 60.f));
    TimeStr = FString::Printf(TEXT("%02d:%02d"), H, M);

    return FString::Printf(TEXT("%s %s %s %.0f\xc2\xb0C"),
        *TimeStr, *Icon, *WeatherLabel, TemperatureC);
}

FString UVNOWeatherSubsystem::GetEffectsDescription() const
{
    TArray<FString> Effects;

    if (StaminaRegenMod < 1.f)
        Effects.Add(FString::Printf(TEXT("Stamina -%.0f%%"), (1.f - StaminaRegenMod) * 100.f));
    if (PhotoScoreBonus > 0.f)
        Effects.Add(FString::Printf(TEXT("Photo +%.0f%%"), PhotoScoreBonus * 100.f));
    else if (PhotoScoreBonus < 0.f)
        Effects.Add(FString::Printf(TEXT("Photo %.0f%%"), PhotoScoreBonus * 100.f));
    if (FogDensity > 0.f)
        Effects.Add(TEXT("Tam nhin giam"));

    if (Effects.Num() == 0)
        return TEXT("Khong co hieu ung dac biet");

    return FString::Join(Effects, TEXT(" | "));
}

void UVNOWeatherSubsystem::FetchWeather()
{
    if (!bApiConfigured) return;

    FString Url = FString::Printf(
        TEXT("https://api.openweathermap.org/data/2.5/weather?lat=%.4f&lon=%.4f&appid=%s&units=metric&lang=vi"),
        ZoneLat, ZoneLng, *ApiKey);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb(TEXT("GET"));
    Request->OnProcessRequestComplete().BindUObject(this, &UVNOWeatherSubsystem::OnWeatherResponse);
    Request->ProcessRequest();
}

void UVNOWeatherSubsystem::OnWeatherResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    if (!bSuccess || !Response.IsValid() || Response->GetResponseCode() != 200) return;

    TSharedPtr<FJsonObject> Root;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid()) return;

    float Temp = 32.f;
    const TSharedPtr<FJsonObject>* MainObj;
    if (Root->TryGetObjectField(TEXT("main"), MainObj))
    {
        (*MainObj)->TryGetNumberField(TEXT("temp"), Temp);
    }

    FString Main = TEXT("Clear");
    int32 WeatherId = 800;
    float WindSpeed = 5.f;
    const TArray<TSharedPtr<FJsonValue>>* WeatherArr;
    if (Root->TryGetArrayField(TEXT("weather"), WeatherArr) && WeatherArr->Num() > 0)
    {
        const TSharedPtr<FJsonObject>* WObj;
        if ((*WeatherArr)[0]->TryGetObject(WObj))
        {
            (*WObj)->TryGetStringField(TEXT("main"), Main);
            (*WObj)->TryGetNumberField(TEXT("id"), WeatherId);
            FString Desc;
            if ((*WObj)->TryGetStringField(TEXT("description"), Desc))
                WeatherLabel = Desc;
        }
    }

    const TSharedPtr<FJsonObject>* WindObj;
    if (Root->TryGetObjectField(TEXT("wind"), WindObj))
    {
        double SpeedMs = 0.0;
        (*WindObj)->TryGetNumberField(TEXT("speed"), SpeedMs);
        WindSpeed = static_cast<float>(SpeedMs * 3.6); // m/s → km/h
    }

    TemperatureC = Temp;
    WindSpeedKmh = WindSpeed;

    // Map OpenWeatherMap to WeatherType
    vno::WeatherType NewType;
    if (Main == TEXT("Thunderstorm") || (WeatherId >= 200 && WeatherId < 300))
        NewType = vno::WeatherType::STORM;
    else if (Main == TEXT("Drizzle") || (Main == TEXT("Rain") && WeatherId < 522))
        NewType = vno::WeatherType::RAIN_LIGHT;
    else if (Main == TEXT("Rain"))
        NewType = vno::WeatherType::RAIN_HEAVY;
    else if (Main == TEXT("Fog") || Main == TEXT("Mist") || Main == TEXT("Haze"))
        NewType = vno::WeatherType::FOG;
    else if (Main == TEXT("Clouds"))
        NewType = vno::WeatherType::CLOUDY;
    else if (Main == TEXT("Clear") && Temp >= 34.f)
        NewType = vno::WeatherType::SUNNY_HOT;
    else
        NewType = vno::WeatherType::SUNNY_CLEAR;

    if (NewType != CurrentType || !bFirstSyncDone)
    {
        TargetType = NewType;
        CurrentType = NewType;
        WeatherTransitionAlpha = 0.f;
        bFirstSyncDone = true;

        // Apply immediate visual effects with transition
        float TargetRain = (NewType == vno::WeatherType::RAIN_LIGHT) ? 0.3f :
                          (NewType == vno::WeatherType::RAIN_HEAVY || NewType == vno::WeatherType::STORM) ? 0.8f : 0.f;
        float TargetFog = (NewType == vno::WeatherType::FOG) ? 0.7f : 0.f;
        float TargetVis = (NewType == vno::WeatherType::FOG) ? 50000.f : 999900.f;

        // Start transition from current values
        RainIntensity = TargetRain * 0.1f; // start from near-zero for smoother visual
        FogDensity = TargetFog * 0.1f;
        VisibilityRangeCm = (NewType == vno::WeatherType::FOG) ? 500000.f : 999900.f;
    }

    UE_LOG(LogTemp, Log, TEXT("[FR-11] OpenWeatherMap: %s %s %.0fC (wind %.0f km/h)"),
        *WeatherLabel, UTF8_TO_TCHAR(vno::weatherName(NewType)), Temp, WindSpeed);
}
