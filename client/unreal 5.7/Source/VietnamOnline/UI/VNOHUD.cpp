#include "UI/VNOHUD.h"
#include "Player/VNOPlayerCharacter.h"
#include "POI/VNOPOIActor.h"
#include "Quest/VNOQuestSubsystem.h"
#include "Weather/VNOWeatherSubsystem.h"
#include "Social/VNOSocialSubsystem.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

void AVNOHUD::BeginPlay()
{
    Super::BeginPlay();
    HUDVersion = 0.f;
    PrimaryActorTick.bCanEverTick = true;
}

void AVNOHUD::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Refresh cached data every N frames
    ThrottleCounter++;
    if (ThrottleCounter < THROTTLE_SKIP) return;
    ThrottleCounter = 0;

    // Cache weather string
    if (UWorld* World = GetWorld())
    {
        if (UVNOWeatherSubsystem* W = World->GetSubsystem<UVNOWeatherSubsystem>())
            CachedWeatherString = W->GetDisplayString();
        if (UVNOQuestSubsystem* Q = World->GetSubsystem<UVNOQuestSubsystem>())
        {
            CachedBottomLeftString = TEXT("Quest: (none) - press J");
            for (const FVNOQuestRuntime& R : Q->GetQuests())
            {
                if (R.bActive)
                {
                    CachedBottomLeftString = FString::Printf(TEXT("Quest: %s [%d/%d]"), *R.Title, R.StepsDone, R.StepsTotal);
                    break;
                }
            }
        }
    }

    // Cache nearby POIs for minimap (only if minimap visible)
    if (AVNOPlayerCharacter* Player = Cast<AVNOPlayerCharacter>(GetOwningPawn()))
    {
        if (Player->IsMinimapVisible())
        {
            CachedPlayerLocation = Player->GetActorLocation();
            RefreshMinimapPOIs();
        }
    }
}

void AVNOHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas) return;

    APawn* Pawn = GetOwningPawn();
    AVNOPlayerCharacter* Player = Cast<AVNOPlayerCharacter>(Pawn);
    if (Player && Player->IsPhotoModeActive()) return;

    const float Pad = 12.f;
    const float ScreenW = Canvas->SizeX;
    const float ScreenH = Canvas->SizeY;

    // === GOC TRAI TREN: Stamina + VND ===
    if (Player)
    {
        FString VndStr;
        if (Player->GetVndBalance() >= 1000000)
            VndStr = FString::Printf(TEXT("VND: %.1fM"), Player->GetVndBalance() / 1000000.f);
        else if (Player->GetVndBalance() >= 1000)
            VndStr = FString::Printf(TEXT("VND: %lldK"), Player->GetVndBalance() / 1000);
        else
            VndStr = FString::Printf(TEXT("VND: %lld"), Player->GetVndBalance());
        DrawCornerPanel(VndStr, Pad, Pad, FLinearColor(0.1f, 0.08f, 0.f, 0.5f));
        DrawStaminaBar(Pad, 48.f, Player->GetStaminaPercent());
    }

    // === GOC PHAI TREN: Zone + Weather (with icon + time) ===
    FString TopRight = BuildTopRightText();
    FLinearColor TopRightBg = FLinearColor(0.f, 0.f, 0.f, 0.5f);
    if (Player)
    {
        UWorld* World = GetWorld();
        if (World)
        {
            if (UVNOWeatherSubsystem* W = World->GetSubsystem<UVNOWeatherSubsystem>())
            {
                if (W->GetWeatherType() == vno::WeatherType::RAIN_HEAVY ||
                    W->GetWeatherType() == vno::WeatherType::STORM)
                    TopRightBg = FLinearColor(0.1f, 0.05f, 0.05f, 0.6f);
                else if (W->GetWeatherType() == vno::WeatherType::SUNNY_HOT)
                    TopRightBg = FLinearColor(0.15f, 0.1f, 0.0f, 0.6f);
                else if (W->GetWeatherType() == vno::WeatherType::FOG)
                    TopRightBg = FLinearColor(0.1f, 0.1f, 0.12f, 0.6f);
            }
        }
    }
    DrawCornerPanel(TopRight, ScreenW - 260.f, Pad, TopRightBg);

    // === TRAI: Active Buffs ===
    if (Player)
    {
        DrawActiveBuffs(Pad, 70.f);
    }

    // === GOC TRAI DUOI: Active Quest ===
    DrawCornerPanel(BuildBottomLeftText(), Pad, ScreenH - 80.f, FLinearColor(0.f, 0.f, 0.1f, 0.5f));

    // === TRAI DUOI: Chat overlay ===
    if (bChatVisible)
    {
        DrawChatOverlay(Pad, ScreenH - 220.f, 300.f);
    }

    // === GOC PHAI DUOI: Minimap ===
    if (Player && Player->IsMinimapVisible())
    {
        float MX = ScreenW - MinimapSize - Pad;
        float MY = ScreenH - MinimapSize - Pad;
        DrawMinimap(MX, MY, MinimapSize);
    }

    // === Party info (if in party) ===
    DrawPartyInfo(ScreenW / 2.f - 80.f, Pad);

    // === PHAI DUOI: Credits ===
    DrawText(TEXT("(c) OpenStreetMap | Cesium Ion"),
        FLinearColor(0.6f, 0.6f, 0.6f, 0.7f), ScreenW * 0.5f - 100.f, ScreenH - 16.f, nullptr, 0.6f);
}

void AVNOHUD::DrawCornerPanel(const FString& Text, float X, float Y, const FLinearColor& Bg)
{
    float TextWidth = 0.f, TextHeight = 0.f;
    GetTextSize(Text, TextWidth, TextHeight, GEngine->GetMediumFont(), 1.f);
    FCanvasTileItem BgTile(FVector2D(X - 6.f, Y - 4.f), FVector2D(TextWidth + 12.f, TextHeight + 8.f), Bg);
    Canvas->DrawItem(BgTile);
    DrawText(Text, FLinearColor::White, X, Y, GEngine->GetMediumFont(), 1.f);
}

void AVNOHUD::DrawStaminaBar(float X, float Y, float Percent)
{
    const float BarWidth = 120.f;
    const float BarHeight = 12.f;

    // Background
    FCanvasTileItem Bg(FVector2D(X, Y), FVector2D(BarWidth, BarHeight),
        FLinearColor(0.1f, 0.1f, 0.1f, 0.6f));
    Canvas->DrawItem(Bg);

    // Fill
    FLinearColor FillColor;
    if (Percent > 0.5f) FillColor = FLinearColor(0.0f, 0.6f, 0.2f, 0.9f);
    else if (Percent > 0.2f) FillColor = FLinearColor(0.8f, 0.6f, 0.0f, 0.9f);
    else FillColor = FLinearColor(0.8f, 0.1f, 0.1f, 0.9f);

    FCanvasTileItem Fill(FVector2D(X, Y), FVector2D(BarWidth * FMath::Clamp(Percent, 0.f, 1.f), BarHeight), FillColor);
    Canvas->DrawItem(Fill);

    // Label
    DrawText(FString::Printf(TEXT("STAMINA %.0f%%"), Percent * 100.f),
        FLinearColor::White, X + 4.f, Y + 1.f, nullptr, 0.7f);
}

void AVNOHUD::DrawMinimap(float X, float Y, float Size)
{
    // Paper-map style background
    FCanvasTileItem Bg(FVector2D(X, Y), FVector2D(Size, Size),
        FLinearColor(0.2f, 0.16f, 0.1f, 0.85f));
    Bg.BlendMode = SE_BLEND_Translucent;
    Canvas->DrawItem(Bg);

    // Border - gold/brown like old map
    FCanvasBoxItem Border(FVector2D(X, Y), FVector2D(Size, Size));
    Border.SetColor(FLinearColor(0.6f, 0.45f, 0.2f, 0.9f));
    Border.LineThickness = 2.f;
    Canvas->DrawItem(Border);

    // Compass rose at top
    FVector2D Center(X + Size * 0.5f, Y + 12.f);
    DrawText(TEXT("N"), FLinearColor(0.8f, 0.7f, 0.4f, 0.9f), Center.X - 5.f, Center.Y - 8.f, nullptr, 0.8f);

    // Center dot for player
    FCanvasTileItem PlayerDot(
        FVector2D(X + Size * 0.5f - 3.f, Y + Size * 0.5f - 3.f),
        FVector2D(6.f, 6.f),
        FLinearColor(0.0f, 0.7f, 1.0f, 1.0f));
    Canvas->DrawItem(PlayerDot);

    // Draw nearby POIs from cached list (refreshed in Tick, not every frame)
    if (CachedPOIs.Num() > 0)
    {
        float RadiusCm = 30000.f; // 300m
        float Scale = Size * 0.5f / RadiusCm;

        for (const TObjectPtr<AVNOPOIActor>& POI : CachedPOIs)
        {
            FVector POILoc = POI->GetActorLocation();
            FVector Dir = POILoc - CachedPlayerLocation;
            float DistSq = Dir.SizeSquared2D();
            if (DistSq > RadiusCm * RadiusCm) continue;

            float POIX = X + Size * 0.5f + Dir.X * Scale - 2.f;
            float POIY = Y + Size * 0.5f + Dir.Y * Scale - 2.f;

            FLinearColor CatColor = POI->GetMapColor();
            float DotSize = POI->IsCheckedIn() ? 3.f : 5.f;
            FCanvasTileItem POIDot(FVector2D(POIX - DotSize*0.5f, POIY - DotSize*0.5f),
                FVector2D(DotSize, DotSize), CatColor);
            Canvas->DrawItem(POIDot);
        }
    }

    // Label
    DrawText(TEXT("HOI AN"), FLinearColor(0.7f, 0.55f, 0.25f, 0.8f),
        X + 4.f, Y + Size - 14.f, nullptr, 0.6f);
}

void AVNOHUD::RefreshMinimapPOIs()
{
    if (!GetWorld()) return;

    // Refresh POI list (only called ~12 times/sec instead of every frame)
    CachedPOIs.Empty();
    for (TActorIterator<AVNOPOIActor> It(GetWorld()); It; ++It)
    {
        AVNOPOIActor* POI = *It;
        if (POI)
        {
            float DistSq = FVector::DistSquared2D(CachedPlayerLocation, POI->GetActorLocation());
            if (DistSq <= 900000000.f) // 300m^2 in cm
            {
                CachedPOIs.Add(POI);
            }
        }
    }
}

void AVNOHUD::DrawChatOverlay(float X, float Y, float Width)
{
    UVNOSocialSubsystem* Social = GetWorld() ? GetWorld()->GetSubsystem<UVNOSocialSubsystem>() : nullptr;
    if (!Social) return;

    TArray<FVNOSocialChatMsg> Recent = Social->GetRecentChats(5);
    if (Recent.Num() == 0) return;

    float LineH = 16.f;
    float PanelH = static_cast<float>(Recent.Num()) * LineH + 8.f;

    // Background
    FCanvasTileItem Bg(FVector2D(X, Y), FVector2D(Width, PanelH),
        FLinearColor(0.f, 0.f, 0.f, 0.4f));
    Bg.BlendMode = SE_BLEND_Translucent;
    Canvas->DrawItem(Bg);

    // Chat lines
    float LineY = Y + 4.f;
    for (const FVNOSocialChatMsg& Msg : Recent)
    {
        FString ChannelTag;
        FLinearColor ChanColor = FLinearColor::White;
        if (Msg.Channel == TEXT("PARTY"))  { ChannelTag = TEXT("[P]"); ChanColor = FLinearColor(0.f, 0.8f, 0.2f); }
        else if (Msg.Channel == TEXT("SYSTEM")) { ChannelTag = TEXT("[!]"); ChanColor = FLinearColor(0.9f, 0.7f, 0.f); }
        else if (Msg.Channel == TEXT("WHISPER")) { ChannelTag = TEXT("[W]"); ChanColor = FLinearColor(0.8f, 0.3f, 0.8f); }
        else { ChannelTag = TEXT("[Z]"); ChanColor = FLinearColor(0.6f, 0.8f, 1.f); }

        FString Display = FString::Printf(TEXT("%s %s: %s"), *ChannelTag, *Msg.SenderName, *Msg.Content);
        DrawText(Display, ChanColor, X + 4.f, LineY, nullptr, 0.65f);
        LineY += LineH;
    }

    // Hint
    DrawText(TEXT("Enter: chat | T: party | U: friends"),
        FLinearColor(0.5f, 0.5f, 0.5f, 0.7f), X + 4.f, Y + PanelH + 2.f, nullptr, 0.55f);
}

void AVNOHUD::DrawPartyInfo(float X, float Y)
{
    UVNOSocialSubsystem* Social = GetWorld() ? GetWorld()->GetSubsystem<UVNOSocialSubsystem>() : nullptr;
    if (!Social || !Social->IsInParty()) return;

    const FVNOPartyInfo& Party = Social->GetCurrentParty();

    FString PartyStr = FString::Printf(TEXT("Party: %s [%d]"), *Party.LeaderName, Party.MemberCount);
    DrawCornerPanel(PartyStr, X, Y, FLinearColor(0.f, 0.15f, 0.f, 0.6f));
}

void AVNOHUD::DrawActiveBuffs(float X, float Y)
{
    AVNOPlayerCharacter* Player = Cast<AVNOPlayerCharacter>(GetOwningPawn());
    if (!Player) return;

    // Access buff arrays — we need to reach the private arrays.
    // Instead, use the player's stamina/speed multipliers as indicators,
    // and display a simple buff list from a public method.
    // Since the arrays are protected, we expose a formatted buff string.

    // Check if any buff is active via multipliers
    TArray<FString> BuffLines;
    float RegMul = Player->GetStaminaRegenMultiplier();
    float SpdMul = Player->GetSpeedMultiplier();
    float ExpMul = Player->GetExpMultiplier();

    if (RegMul > 1.0f)
        BuffLines.Add(FString::Printf(TEXT("[Stamina] +%.0f%%"), (RegMul - 1.0f) * 100.f));
    if (SpdMul > 1.0f)
        BuffLines.Add(FString::Printf(TEXT("[Speed] +%.0f%%"), (SpdMul - 1.0f) * 100.f));
    if (ExpMul > 1.0f)
        BuffLines.Add(FString::Printf(TEXT("[EXP] +%.0f%%"), (ExpMul - 1.0f) * 100.f));

    if (BuffLines.Num() == 0) return;

    // Background panel
    float PanelW = 180.f;
    float PanelH = static_cast<float>(BuffLines.Num()) * 20.f + 12.f;
    FCanvasTileItem Bg(FVector2D(X, Y), FVector2D(PanelW, PanelH),
        FLinearColor(0.0f, 0.15f, 0.0f, 0.6f));
    Bg.BlendMode = SE_BLEND_Translucent;
    Canvas->DrawItem(Bg);

    // Buff lines
    float LineY = Y + 6.f;
    for (const FString& Line : BuffLines)
    {
        DrawText(Line, FLinearColor(0.3f, 1.0f, 0.3f, 0.95f),
            X + 6.f, LineY, nullptr, 0.7f);
        LineY += 20.f;
    }
}

FString AVNOHUD::BuildTopRightText()
{
    if (!CachedWeatherString.IsEmpty())
        return FString::Printf(TEXT("HOI_AN | %s"), *CachedWeatherString);
    return TEXT("HOI_AN | Nang dep 32C");
}

FString AVNOHUD::BuildBottomLeftText()
{
    if (!CachedBottomLeftString.IsEmpty())
        return CachedBottomLeftString;
    return TEXT("Quest: (none) - press J");
}
