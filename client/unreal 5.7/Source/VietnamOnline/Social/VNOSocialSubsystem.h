#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "VNOSocialSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FVNOSocialChatMsg
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString SenderName;

    UPROPERTY(BlueprintReadOnly)
    FString Content;

    UPROPERTY(BlueprintReadOnly)
    FString Channel; // PROXIMITY, PARTY, ZONE, WHISPER, SYSTEM

    UPROPERTY(BlueprintReadOnly)
    float TimeReceived = 0.f;
};

USTRUCT(BlueprintType)
struct FVNOPlayerInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString PlayerId;

    UPROPERTY(BlueprintReadOnly)
    FString DisplayName;

    UPROPERTY(BlueprintReadOnly)
    int32 ExplorerLevel = 1;

    UPROPERTY(BlueprintReadOnly)
    bool bIsFriend = false;

    UPROPERTY(BlueprintReadOnly)
    bool bInParty = false;
};

USTRUCT(BlueprintType)
struct FVNOPartyInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString PartyId;

    UPROPERTY(BlueprintReadOnly)
    FString LeaderName;

    UPROPERTY(BlueprintReadOnly)
    TArray<FVNOPlayerInfo> Members;

    UPROPERTY(BlueprintReadOnly)
    int32 MemberCount = 0;
};

/** FR-05: Social system — chat, party, friends, online players */
UCLASS()
class VIETNAMONLINE_API UVNOSocialSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // === CHAT ===
    UFUNCTION(BlueprintCallable, Category = "VNO|Social")
    void SendChat(const FString& Content, const FString& Channel = TEXT("PROXIMITY"));

    UFUNCTION(BlueprintPure, Category = "VNO|Social")
    const TArray<FVNOSocialChatMsg>& GetChatHistory() const { return ChatHistory; }

    UFUNCTION(BlueprintPure, Category = "VNO|Social")
    TArray<FVNOSocialChatMsg> GetRecentChats(int32 Count = 10) const;

    UFUNCTION(BlueprintCallable, Category = "VNO|Social")
    void AddSystemMessage(const FString& Message);

    // === PARTY ===
    UFUNCTION(BlueprintCallable, Category = "VNO|Social")
    bool CreateParty();

    UFUNCTION(BlueprintCallable, Category = "VNO|Social")
    bool JoinParty(const FString& PartyId);

    UFUNCTION(BlueprintCallable, Category = "VNO|Social")
    bool LeaveParty();

    UFUNCTION(BlueprintPure, Category = "VNO|Social")
    bool IsInParty() const { return CurrentParty.MemberCount > 0; }

    UFUNCTION(BlueprintPure, Category = "VNO|Social")
    const FVNOPartyInfo& GetCurrentParty() const { return CurrentParty; }

    // === FRIENDS ===
    UFUNCTION(BlueprintCallable, Category = "VNO|Social")
    bool AddFriend(const FString& PlayerId, const FString& DisplayName);

    UFUNCTION(BlueprintCallable, Category = "VNO|Social")
    bool RemoveFriend(const FString& PlayerId);

    UFUNCTION(BlueprintPure, Category = "VNO|Social")
    const TArray<FVNOPlayerInfo>& GetFriends() const { return Friends; }

    UFUNCTION(BlueprintPure, Category = "VNO|Social")
    bool IsFriend(const FString& PlayerId) const;

    // === ONLINE PLAYERS ===
    UFUNCTION(BlueprintPure, Category = "VNO|Social")
    const TArray<FVNOPlayerInfo>& GetOnlinePlayers() const { return OnlinePlayers; }

    UFUNCTION(BlueprintCallable, Category = "VNO|Social")
    void UpdateOnlinePlayers(const TArray<FVNOPlayerInfo>& Players);

private:
    UPROPERTY()
    TArray<FVNOSocialChatMsg> ChatHistory;

    UPROPERTY()
    TArray<FVNOPlayerInfo> Friends;

    UPROPERTY()
    TArray<FVNOPlayerInfo> OnlinePlayers;

    UPROPERTY()
    FVNOPartyInfo CurrentParty;

    static constexpr int32 MAX_CHAT_HISTORY = 50;
};
