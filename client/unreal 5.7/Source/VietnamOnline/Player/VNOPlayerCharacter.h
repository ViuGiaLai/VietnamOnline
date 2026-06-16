#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Network/VNOWebSocketClient.h"
#include "VNOPlayerCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UVNOChatWidget;
class UVNOChatBridge;

UCLASS()
class VIETNAMONLINE_API AVNOPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AVNOPlayerCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Replication
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Callbacks for replicated properties
    void OnRep_ReplicatedMovement();

    UFUNCTION()
    void OnRep_MovementState();

    UFUNCTION()
    void OnRep_VehicleType();

    UFUNCTION(BlueprintPure, Category = "VNO|Player")
    float GetStaminaPercent() const;

    UFUNCTION(BlueprintPure, Category = "VNO|Player")
    int64 GetVndBalance() const { return VndBalance; }

    UFUNCTION(BlueprintPure, Category = "VNO|Player")
    bool IsPhotoModeActive() const { return bPhotoMode; }

    UFUNCTION(BlueprintPure, Category = "VNO|Player")
    bool IsSprinting() const { return bIsSprinting; }

    UFUNCTION(BlueprintPure, Category = "VNO|Buff")
    float GetStaminaRegenMultiplier() const { return StaminaRegenMultiplier; }

    UFUNCTION(BlueprintPure, Category = "VNO|Buff")
    float GetSpeedMultiplier() const { return SpeedMultiplier; }

    UFUNCTION(BlueprintPure, Category = "VNO|Buff")
    float GetExpMultiplier() const { return ExpMultiplier; }

    /** Apply a food buff. Type: STAMINA_REGEN, SPEED_BOOST, EXP_BONUS */
    void ApplyBuff(const FString& BuffType, const FString& FoodName, float Value, int32 DurationSec);

    void AddVnd(int64 Amount) { VndBalance += Amount; }

    void SetVndBalance(int64 Amount) { VndBalance = Amount; }

    UFUNCTION(BlueprintCallable, Category = "VNO|Character")
    void ApplyAppearanceFromState(class AVNOPlayerState* InPlayerState);


    bool IsChatOpen() const { return bChatOpen; }

    UFUNCTION(BlueprintPure, Category = "VNO|UI")
    bool IsMinimapVisible() const { return bShowMinimap; }

    void SetupMannequinMesh();

    // Multiplayer
    UFUNCTION(BlueprintCallable, Category = "VNO|Multiplayer")
    void ConnectToServer(const FString& ServerUrl, int32 Port);

    UFUNCTION(BlueprintCallable, Category = "VNO|Multiplayer")
    void DisconnectFromServer();

    UFUNCTION(BlueprintCallable, Category = "VNO|Multiplayer")
    void SendPositionToServer();

    UFUNCTION(BlueprintPure, Category = "VNO|Multiplayer")
    bool IsOnline() const { return bIsOnline; }

    UFUNCTION(BlueprintPure, Category = "VNO|Multiplayer")
    int32 GetOtherPlayerCount() const { return OtherPlayerCount; }

protected:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void StartSprint();
    void StopSprint();
    void Interact();
    void TogglePhotoMode();
    void CapturePhoto();
    void ToggleMinimap();
    void OpenQuestJournal();
    void OpenChat();
    void OpenParty();
    void OpenFriends();

    void UpdateStamina(float DeltaSeconds);
    void UpdateBuffs(float DeltaSeconds);

    /** Apply a food buff. Type: STAMINA_REGEN, SPEED_BOOST, EXP_BONUS */
    UPROPERTY()
    TArray<FString> ActiveBuffNames;

    UPROPERTY()
    TArray<float> BuffTimeRemaining;

    UPROPERTY()
    TArray<float> BuffValues;

    float StaminaRegenMultiplier = 1.0f;
    float SpeedMultiplier = 1.0f;
    float ExpMultiplier = 1.0f;

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

    // Replicated properties for multiplayer
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "VNO|Multiplayer")
    FVector ReplicatedLocation;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "VNO|Multiplayer")
    FRotator ReplicatedRotation;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "VNO|Multiplayer")
    uint8 ReplicatedMovementState;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "VNO|Multiplayer")
    uint8 ReplicatedVehicleType;

    // Online state
    UPROPERTY(BlueprintReadOnly, Category = "VNO|Multiplayer")
    bool bIsOnline = false;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|Multiplayer")
    int32 OtherPlayerCount = 0;

    // Chat widget
    UPROPERTY()
    TObjectPtr<UVNOChatWidget> ChatWidget;

    bool bChatOpen = false;

    // WebSocket client + chat bridge
    UPROPERTY()
    TObjectPtr<UVNOChatBridge> ChatBridge;

    TObjectPtr<UVNOWebSocketClient> WebSocketClient;

    // Last sync time
    float LastSyncTime = 0.0f;
    static constexpr float SYNC_INTERVAL = 0.05f; // 20Hz

    // Performance: throttle non-essential updates
    int32 ThrottleCounter = 10; // start high so first Tick() runs immediately
    static constexpr int32 THROTTLE_SKIP = 10; // only update stamina/buffs every 10 frames (~6Hz at 60fps)
};
