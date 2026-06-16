#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "VNOSupabaseSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVNOAuthCompleted, bool, bSuccess, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVNOSaveCompleted, bool, bSuccess, const FString&, Message);

/** FR-01 + FR-10 — Supabase Auth & profile REST (mirrors shared SupabaseClient) */
UCLASS()
class VIETNAMONLINE_API UVNOSupabaseSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "VNO|Auth")
    void SignIn(const FString& Email, const FString& Password);

    UFUNCTION(BlueprintCallable, Category = "VNO|Auth")
    void SignUp(const FString& Email, const FString& Password);

    UFUNCTION(BlueprintCallable, Category = "VNO|Save")
    void SaveCharacterProfile(class AVNOPlayerState* PlayerState, class AVNOPlayerCharacter* Character);

    UFUNCTION(BlueprintCallable, Category = "VNO|Save")
    void LoadCharacterProfile(class AVNOPlayerState* PlayerState, class AVNOPlayerCharacter* Character);

    UFUNCTION(BlueprintCallable, Category = "VNO|Save")
    void CloudSaveAll();

    UFUNCTION(BlueprintCallable, Category = "VNO|Save")
    void SaveOnQuit();

    UFUNCTION(BlueprintPure, Category = "VNO|Config")
    bool IsConfigured() const { return bConfigured; }

    UFUNCTION(BlueprintPure, Category = "VNO|Auth")
    bool DevSkipAuth() const { return bDevSkipAuth; }

    UPROPERTY(BlueprintAssignable, Category = "VNO|Auth")
    FVNOAuthCompleted OnAuthCompleted;

    UPROPERTY(BlueprintAssignable, Category = "VNO|Save")
    FVNOSaveCompleted OnSaveCompleted;

private:
    void OnAuthResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess, bool bSignUp);
    void OnSaveResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
    void OnLoadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);

    FString SupabaseUrl;
    FString AnonKey;
    bool bConfigured = false;
    bool bDevSkipAuth = false;

    TWeakObjectPtr<AVNOPlayerState> PendingLoadPlayerState;
    TWeakObjectPtr<AVNOPlayerCharacter> PendingLoadCharacter;
};
