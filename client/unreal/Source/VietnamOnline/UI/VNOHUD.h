#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "VNOHUD.generated.h"

UCLASS()
class VIETNAMONLINE_API AVNOHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

private:
    void DrawCornerPanel(const FString& Text, float X, float Y, const FLinearColor& Bg) const;
    FString BuildTopRightText() const;
    FString BuildBottomLeftText() const;
};
