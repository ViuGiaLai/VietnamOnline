#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "VNOHUD.generated.h"

class AVNOPOIActor;

UCLASS()
class VIETNAMONLINE_API AVNOHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    void DrawCornerPanel(const FString& Text, float X, float Y, const FLinearColor& Bg);
    void DrawMinimap(float X, float Y, float Size);
    void DrawStaminaBar(float X, float Y, float Percent);
    void DrawActiveBuffs(float X, float Y);
    void DrawChatOverlay(float X, float Y, float Width);
    void DrawPartyInfo(float X, float Y);
    FString BuildTopRightText();
    FString BuildBottomLeftText();

    /** Refresh cached POI list for minimap (called on Tick, not every frame draw) */
    void RefreshMinimapPOIs();

    float MinimapSize = 140.f;
    float HUDVersion = 0.f;
    bool bChatVisible = true;

    // === Cached data for performance ===
    UPROPERTY()
    TArray<TObjectPtr<AVNOPOIActor>> CachedPOIs;
    FVector CachedPlayerLocation;
    FString CachedWeatherString;
    FString CachedBottomLeftString;
    int32 ThrottleCounter = 0;
    static constexpr int32 THROTTLE_SKIP = 5; // refresh cache every 5 frames
};
