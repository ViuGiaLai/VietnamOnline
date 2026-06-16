#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "VNOPlayerState.generated.h"

/** FR-02 character data replicated to clients */
UCLASS()
class VIETNAMONLINE_API AVNOPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "VNO|Auth")
    bool IsAuthenticated() const { return !AccessToken.IsEmpty() && !UserId.IsEmpty(); }

    UFUNCTION(BlueprintPure, Category = "VNO|Character")
    bool IsCharacterReady() const { return bCharacterCreated && !DisplayName.IsEmpty(); }

    UPROPERTY(BlueprintReadOnly, Category = "VNO|Auth")
    FString UserId;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|Auth")
    FString AccessToken;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|Character")
    FString DisplayName = TEXT("Du khach");

    UPROPERTY(BlueprintReadOnly, Category = "VNO|Character")
    FString Gender = TEXT("nam");

    UPROPERTY(BlueprintReadOnly, Category = "VNO|Character")
    FString SkinTone = TEXT("medium");

    UPROPERTY(BlueprintReadOnly, Category = "VNO|Character")
    bool bCharacterCreated = false;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|Economy")
    int64 VndBalance = 500000;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|Stats")
    int32 ExplorerLevel = 1;
};
