#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "VNOPhotoModeSubsystem.generated.h"

class AVNOPlayerCharacter;
class AVNOPOIActor;

USTRUCT(BlueprintType)
struct FVNOPhotoEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString PhotoId;

    UPROPERTY(BlueprintReadOnly)
    FString Title;

    UPROPERTY(BlueprintReadOnly)
    FString LocationName;

    UPROPERTY(BlueprintReadOnly)
    float TechnicalScore = 70.f;

    UPROPERTY(BlueprintReadOnly)
    int32 Timestamp;

    UPROPERTY(BlueprintReadOnly)
    FString ScreenshotPath;

    UPROPERTY(BlueprintReadOnly)
    FVector Location;
};

/** K.1 — Photo Mode: screenshot capture, gallery, scoring */
UCLASS()
class VIETNAMONLINE_API UVNOPhotoModeSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "VNO|Photo")
    void CapturePhoto(AVNOPlayerCharacter* Player);

    UFUNCTION(BlueprintCallable, Category = "VNO|Photo")
    void OpenGallery();

    UFUNCTION(BlueprintPure, Category = "VNO|Photo")
    const TArray<FVNOPhotoEntry>& GetGallery() const { return Gallery; }

    UFUNCTION(BlueprintPure, Category = "VNO|Photo")
    int32 GetPhotoCount() const { return Gallery.Num(); }

    /** Toggle photo mode on/off and return new state */
    UFUNCTION(BlueprintCallable, Category = "VNO|Photo")
    bool TogglePhotoMode(AVNOPlayerCharacter* Player);

    UFUNCTION(BlueprintPure, Category = "VNO|Photo")
    bool IsPhotoModeActive() const { return bPhotoModeActive; }

protected:
    /** Score the captured image based on lighting/time */
    float CalculatePhotoScore(AVNOPlayerCharacter* Player) const;

    /** Find nearest POI for auto-tagging (within 100m) */
    FString FindNearestPOI(AVNOPlayerCharacter* Player) const;

    /** Save gallery metadata to disk */
    void SaveGalleryToDisk();

    /** Load gallery metadata from disk */
    void LoadGalleryFromDisk();

    /** Generate unique photo ID */
    FString GeneratePhotoId() const;

    UPROPERTY()
    TArray<FVNOPhotoEntry> Gallery;

    bool bPhotoModeActive = false;
    FVector SavedCameraLocation;
    FRotator SavedCameraRotation;
    float SavedArmLength = 350.f;
    int32 NextPhotoIndex = 0;
};
