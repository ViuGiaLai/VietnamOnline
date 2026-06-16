#include "PhotoMode/VNOPhotoModeSubsystem.h"
#include "Player/VNOPlayerCharacter.h"
#include "POI/VNOPOIActor.h"
#include "Weather/VNOWeatherSubsystem.h"
#include "Player/VNOPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/JsonTypes.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "EngineUtils.h"
#include "ImageUtils.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"

void UVNOPhotoModeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LoadGalleryFromDisk();
    UE_LOG(LogTemp, Log, TEXT("[Photo] Gallery loaded: %d photos"), Gallery.Num());
}

FString UVNOPhotoModeSubsystem::GeneratePhotoId() const
{
    FDateTime Now = FDateTime::Now();
    return FString::Printf(TEXT("PHOTO-%s-%04d"), *Now.ToString(TEXT("%Y%m%d-%H%M%S")), NextPhotoIndex);
}

FString UVNOPhotoModeSubsystem::FindNearestPOI(AVNOPlayerCharacter* Player) const
{
    if (!Player) return TEXT("");
    FVector PlayerLoc = Player->GetActorLocation();

    FString NearestName;
    float NearestDist = 10000.f; // 100m max

    UWorld* World = GetWorld();
    if (!World) return TEXT("");

    for (TActorIterator<AVNOPOIActor> It(World); It; ++It)
    {
        float Dist = FVector::Dist2D(PlayerLoc, It->GetActorLocation());
        if (Dist < NearestDist)
        {
            NearestDist = Dist;
            NearestName = It->GetPoiName();
        }
    }
    return NearestName;
}

float UVNOPhotoModeSubsystem::CalculatePhotoScore(AVNOPlayerCharacter* Player) const
{
    if (!Player) return 50.f;

    float Score = 70.f; // Base score

    // Time of day bonus (golden hour 17h-18h = +15%)
    FDateTime Now = FDateTime::Now();
    int32 Hour = Now.GetHour();
    if (Hour >= 17 && Hour <= 18) Score += 15.f;
    else if (Hour >= 6 && Hour <= 7) Score += 10.f; // Sunrise bonus

    // Weather bonus (FR-11): dramatic weather = atmospheric photos
    if (UWorld* World = GetWorld())
    {
        if (UVNOWeatherSubsystem* W = World->GetSubsystem<UVNOWeatherSubsystem>())
        {
            Score += W->GetPhotoScoreBonus() * 20.f; // Scale from 0-50% → 0-10 pts
        }
    }

    // Random variance for uniqueness
    Score += FMath::FRandRange(-5.f, 5.f);
    return FMath::Clamp(Score, 0.f, 100.f);
}

void UVNOPhotoModeSubsystem::CapturePhoto(AVNOPlayerCharacter* Player)
{
    if (!Player)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Photo] Cannot capture — no player"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    // Generate photo data
    FString PhotoId = GeneratePhotoId();
    NextPhotoIndex++;

    float Score = CalculatePhotoScore(Player);
    FString NearestPOI = FindNearestPOI(Player);
    FDateTime Now = FDateTime::Now();

    // Take screenshot
    FString ScreenshotDir = FPaths::ProjectSavedDir() + TEXT("Screenshots/VNO/");
    IFileManager::Get().MakeDirectory(*ScreenshotDir, true);

    FString Timestamp = Now.ToString(TEXT("%Y%m%d_%H%M%S"));
    FString Filename = FString::Printf(TEXT("VNO_%s.png"), *Timestamp);
    FString FullPath = ScreenshotDir + Filename;

    // Request screenshot
    FScreenshotRequest::RequestScreenshot(FullPath, false, false);
    UE_LOG(LogTemp, Log, TEXT("[Photo] Screenshot queued: %s"), *FullPath);

    // Create entry
    FVNOPhotoEntry Entry;
    Entry.PhotoId = PhotoId;
    Entry.Timestamp = Now.ToUnixTimestamp();
    Entry.TechnicalScore = Score;
    Entry.LocationName = NearestPOI.IsEmpty() ? TEXT("Hoi An") : NearestPOI;
    Entry.Title = FString::Printf(TEXT("%s @ %s"),
        *Now.ToString(TEXT("%H:%M")), *Entry.LocationName);
    Entry.ScreenshotPath = FullPath;
    Entry.Location = Player->GetActorLocation();

    Gallery.Add(Entry);
    SaveGalleryToDisk();

    // HUD feedback
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
            FString::Printf(TEXT("📸 Chup anh! Score: %.0f/100 — %s"),
                Score, *Entry.LocationName));

    UE_LOG(LogTemp, Log, TEXT("[Photo] Saved: %s (score %.0f, POI: %s)"),
        *PhotoId, Score, *NearestPOI);
}

bool UVNOPhotoModeSubsystem::TogglePhotoMode(AVNOPlayerCharacter* Player)
{
    if (!Player) return false;

    bPhotoModeActive = !bPhotoModeActive;

    if (bPhotoModeActive)
    {
        // Enter photo mode — reset sprint and disable movement
        Player->GetCharacterMovement()->MaxWalkSpeed = 500.f;
        Player->GetCharacterMovement()->DisableMovement();
        UE_LOG(LogTemp, Log, TEXT("[Photo] Mode ON — F8 to exit, Space to capture"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
                TEXT("📷 Photo Mode: F8 = thoat, Space = chup"));
    }
    else
    {
        // Exit photo mode
        Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        UE_LOG(LogTemp, Log, TEXT("[Photo] Mode OFF"));
    }

    return bPhotoModeActive;
}

void UVNOPhotoModeSubsystem::OpenGallery()
{
    int32 Count = Gallery.Num();
    UE_LOG(LogTemp, Log, TEXT("[Photo] Gallery: %d photos"), Count);

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
            FString::Printf(TEXT("📖 Gallery: %d photo(s)"), Count));

    // Show last 5 photos in log
    for (int32 i = FMath::Max(0, Count - 5); i < Count; ++i)
    {
        UE_LOG(LogTemp, Log, TEXT("  [%d] %s — score %.0f — %s"),
            i + 1, *Gallery[i].Title, Gallery[i].TechnicalScore, *Gallery[i].ScreenshotPath);
    }
}

void UVNOPhotoModeSubsystem::SaveGalleryToDisk()
{
    TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();
    TArray<TSharedPtr<FJsonValue>> PhotoArray;

    for (const FVNOPhotoEntry& Entry : Gallery)
    {
        TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
        Obj->SetStringField(TEXT("id"), Entry.PhotoId);
        Obj->SetStringField(TEXT("title"), Entry.Title);
        Obj->SetStringField(TEXT("location"), Entry.LocationName);
        Obj->SetNumberField(TEXT("score"), Entry.TechnicalScore);
        Obj->SetNumberField(TEXT("timestamp"), Entry.Timestamp);
        Obj->SetStringField(TEXT("path"), Entry.ScreenshotPath);
        PhotoArray.Add(MakeShared<FJsonValueObject>(Obj));
    }

    Root->SetArrayField(TEXT("photos"), PhotoArray);

    FString OutputStr;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputStr);
    FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);

    FString GalleryPath = FPaths::ProjectSavedDir() + TEXT("Gallery/gallery.json");
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(GalleryPath), true);
    FFileHelper::SaveStringToFile(OutputStr, *GalleryPath);
}

void UVNOPhotoModeSubsystem::LoadGalleryFromDisk()
{
    Gallery.Empty();

    FString GalleryPath = FPaths::ProjectSavedDir() + TEXT("Gallery/gallery.json");
    if (!FPaths::FileExists(GalleryPath)) return;

    FString Content;
    if (!FFileHelper::LoadFileToString(Content, *GalleryPath)) return;

    TSharedPtr<FJsonObject> Root;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid()) return;

    const TArray<TSharedPtr<FJsonValue>>* PhotoArray;
    if (!Root->TryGetArrayField(TEXT("photos"), PhotoArray)) return;

    for (const TSharedPtr<FJsonValue>& Val : *PhotoArray)
    {
        TSharedPtr<FJsonObject> Obj = Val->AsObject();
        if (!Obj) continue;

        FVNOPhotoEntry Entry;
        Entry.PhotoId = Obj->GetStringField(TEXT("id"));
        Entry.Title = Obj->GetStringField(TEXT("title"));
        Entry.LocationName = Obj->GetStringField(TEXT("location"));
        Entry.TechnicalScore = static_cast<float>(Obj->GetNumberField(TEXT("score")));
        Entry.Timestamp = static_cast<int32>(Obj->GetNumberField(TEXT("timestamp")));

        if (Obj->TryGetStringField(TEXT("path"), Entry.ScreenshotPath))
        {
            if (FPaths::FileExists(Entry.ScreenshotPath))
                Gallery.Add(Entry);
        }
    }
}
