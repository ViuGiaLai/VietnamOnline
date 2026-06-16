#include "UI/VNOHUD.h"
#include "Player/VNOPlayerCharacter.h"
#include "Quest/VNOQuestSubsystem.h"
#include "Weather/VNOWeatherSubsystem.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

void AVNOHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas) return;

    APawn* Pawn = GetOwningPawn();
    AVNOPlayerCharacter* Player = Cast<AVNOPlayerCharacter>(Pawn);
    if (Player && Player->IsPhotoModeActive()) return;

    const float Pad = 12.f;
    const float W = Canvas->SizeX;
    const float H = Canvas->SizeY;

    DrawCornerPanel(BuildTopRightText(), W - 280.f, Pad, FLinearColor(0.f, 0.f, 0.f, 0.45f));
    DrawCornerPanel(BuildBottomLeftText(), Pad, H - 120.f, FLinearColor(0.f, 0.f, 0.f, 0.45f));

    if (Player)
    {
        FString Stamina = FString::Printf(TEXT("Stamina: %.0f%%"), Player->GetStaminaPercent() * 100.f);
        DrawCornerPanel(Stamina, Pad, Pad, FLinearColor(0.f, 0.1f, 0.f, 0.45f));

        FString Vnd = FString::Printf(TEXT("VND: %lld"), Player->GetVndBalance());
        DrawCornerPanel(Vnd, Pad, 50.f, FLinearColor(0.1f, 0.08f, 0.f, 0.45f));

        if (Player->IsMinimapVisible())
        {
            const float MapSize = 140.f;
            float MX = W - MapSize - Pad;
            float MY = H - MapSize - Pad;
            FCanvasTileItem Tile(FVector2D(MX, MY), FVector2D(MapSize, MapSize),
                FLinearColor(0.15f, 0.12f, 0.08f, 0.7f));
            Canvas->DrawItem(Tile);
            DrawText(TEXT("MINIMAP"), FLinearColor::White, MX + 30.f, MY + 60.f, GEngine->GetMediumFont());
        }
    }

    DrawText(TEXT("(c) OpenStreetMap | Cesium Ion — doc license"),
        FLinearColor(0.6f, 0.6f, 0.6f, 0.8f), W * 0.5f - 120.f, H - 20.f, nullptr, 0.7f);
}

void AVNOHUD::DrawCornerPanel(const FString& Text, float X, float Y, const FLinearColor& Bg) const
{
    FVector2D Size;
    GetTextSize(Text, Size.X, Size.Y, GEngine->GetMediumFont(), 1.f);
    FCanvasTileItem BgTile(FVector2D(X - 6.f, Y - 4.f), FVector2D(Size.X + 12.f, Size.Y + 8.f), Bg);
    Canvas->DrawItem(BgTile);
    DrawText(Text, FLinearColor::White, X, Y, GEngine->GetMediumFont(), 1.f);
}

FString AVNOHUD::BuildTopRightText() const
{
    FString Weather = TEXT("Nang dep 32C");
    if (UWorld* World = GetWorld())
    {
        if (UVNOWeatherSubsystem* W = World->GetSubsystem<UVNOWeatherSubsystem>())
        {
            Weather = W->GetDisplayString();
        }
    }
    return FString::Printf(TEXT("HOI_AN | %s"), *Weather);
}

FString AVNOHUD::BuildBottomLeftText() const
{
    if (UWorld* World = GetWorld())
    {
        if (UVNOQuestSubsystem* Q = World->GetSubsystem<UVNOQuestSubsystem>())
        {
            for (const FVNOQuestRuntime& R : Q->GetQuests())
            {
                if (R.bActive)
                    return FString::Printf(TEXT("Quest: %s [%d/%d]"), *R.Title, R.StepsDone, R.StepsTotal);
            }
        }
    }
    return TEXT("Quest: (none) — press J");
}
