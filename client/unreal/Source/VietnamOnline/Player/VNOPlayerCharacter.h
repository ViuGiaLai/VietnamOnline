#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VNOPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class VIETNAMONLINE_API AVNOPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AVNOPlayerCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintPure, Category = "VNO|Player")
    float GetStaminaPercent() const;

    UFUNCTION(BlueprintPure, Category = "VNO|Player")
    int64 GetVndBalance() const { return VndBalance; }

    UFUNCTION(BlueprintPure, Category = "VNO|Player")
    bool IsPhotoModeActive() const { return bPhotoMode; }

    UFUNCTION(BlueprintPure, Category = "VNO|Player")
    bool IsSprinting() const { return bIsSprinting; }

    void AddVnd(int64 Amount) { VndBalance += Amount; }

    void SetVndBalance(int64 Amount) { VndBalance = Amount; }

    UFUNCTION(BlueprintCallable, Category = "VNO|Character")
    void ApplyAppearanceFromState(class AVNOPlayerState* PlayerState);

    void SetupMannequinMesh();

protected:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void StartSprint();
    void StopSprint();
    void Interact();
    void TogglePhotoMode();
    void ToggleMinimap();
    void OpenQuestJournal();

    void UpdateStamina(float DeltaSeconds);

    UPROPERTY(VisibleAnywhere, Category = "VNO|Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, Category = "VNO|Camera")
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(EditAnywhere, Category = "VNO|Movement")
    float WalkSpeed = 500.f;

    UPROPERTY(EditAnywhere, Category = "VNO|Movement")
    float SprintSpeed = 900.f;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|Player")
    float StaminaCurrent = 100.f;

    UPROPERTY(EditAnywhere, Category = "VNO|Player")
    float StaminaMax = 100.f;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|Player")
    int64 VndBalance = 500000;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|Player")
    bool bIsSprinting = false;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|Player")
    bool bPhotoMode = false;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|UI")
    bool bShowMinimap = true;

    UFUNCTION(BlueprintPure, Category = "VNO|UI")
    bool IsMinimapVisible() const { return bShowMinimap; }
};
