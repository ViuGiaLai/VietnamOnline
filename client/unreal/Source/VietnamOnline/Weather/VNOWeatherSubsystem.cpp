#include "Weather/VNOWeatherSubsystem.h"
#include "Core/VNOGameInstance.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

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
        }
    }
    FetchWeather();
}

void UVNOWeatherSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UVNOWeatherSubsystem::Tick(float DeltaTime)
{
    SyncTimer += DeltaTime;
    if (SyncTimer >= SyncIntervalSec)
    {
        SyncTimer = 0.f;
        FetchWeather();
    }
}

TStatId UVNOWeatherSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UVNOWeatherSubsystem, STATGROUP_Tickables);
}

FString UVNOWeatherSubsystem::GetDisplayString() const
{
    return FString::Printf(TEXT("%s %.0fC"), *WeatherLabel, TemperatureC);
}

void UVNOWeatherSubsystem::FetchWeather()
{
    if (ApiKey.IsEmpty()) return;

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

    const TSharedPtr<FJsonObject>* MainObj;
    if (Root->TryGetObjectField(TEXT("main"), MainObj))
    {
        (*MainObj)->TryGetNumberField(TEXT("temp"), TemperatureC);
    }

    const TArray<TSharedPtr<FJsonValue>>* WeatherArr;
    if (Root->TryGetArrayField(TEXT("weather"), WeatherArr) && WeatherArr->Num() > 0)
    {
        const TSharedPtr<FJsonObject>* WObj;
        if ((*WeatherArr)[0]->TryGetObject(WObj))
        {
            FString Desc;
            if ((*WObj)->TryGetStringField(TEXT("description"), Desc))
            {
                WeatherLabel = Desc;
            }
        }
    }
    UE_LOG(LogTemp, Log, TEXT("[Weather] OpenWeatherMap: %s"), *GetDisplayString());
}
