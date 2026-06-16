#include "Social/VNOSocialSubsystem.h"
#include "Player/VNOPlayerState.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UVNOSocialSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Log, TEXT("[FR-05] Social subsystem initialized"));
}

void UVNOSocialSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

// === CHAT ===

void UVNOSocialSubsystem::SendChat(const FString& Content, const FString& Channel)
{
    if (Content.IsEmpty()) return;

    // Get sender name from player state
    FString SenderName = TEXT("Ban");
    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            if (AVNOPlayerState* PS = PC->GetPlayerState<AVNOPlayerState>())
            {
                SenderName = PS->DisplayName;
                if (SenderName.IsEmpty()) SenderName = TEXT("Ban");
            }
        }
    }

    FVNOSocialChatMsg Msg;
    Msg.SenderName = SenderName;
    Msg.Content = Content;
    Msg.Channel = Channel;
    Msg.TimeReceived = static_cast<float>(FDateTime::Now().GetSecond());

    ChatHistory.Add(Msg);
    if (ChatHistory.Num() > MAX_CHAT_HISTORY)
        ChatHistory.RemoveAt(0);

    // Show on screen
    if (GEngine)
    {
        FColor ChanColor = FColor::White;
        if (Channel == TEXT("PARTY")) ChanColor = FColor::Green;
        else if (Channel == TEXT("SYSTEM")) ChanColor = FColor::Yellow;
        else if (Channel == TEXT("WHISPER")) ChanColor = FColor::Magenta;

        GEngine->AddOnScreenDebugMessage(-1, 6.f, ChanColor,
            FString::Printf(TEXT("[%s] %s: %s"), *Channel, *SenderName, *Content));
    }

    UE_LOG(LogTemp, Log, TEXT("[Chat][%s] %s: %s"), *Channel, *SenderName, *Content);
}

TArray<FVNOSocialChatMsg> UVNOSocialSubsystem::GetRecentChats(int32 Count) const
{
    TArray<FVNOSocialChatMsg> Recent;
    int32 Start = FMath::Max(0, ChatHistory.Num() - Count);
    for (int32 i = Start; i < ChatHistory.Num(); ++i)
        Recent.Add(ChatHistory[i]);
    return Recent;
}

void UVNOSocialSubsystem::AddSystemMessage(const FString& Message)
{
    FVNOSocialChatMsg Msg;
    Msg.SenderName = TEXT("Hoi An");
    Msg.Content = Message;
    Msg.Channel = TEXT("SYSTEM");
    Msg.TimeReceived = static_cast<float>(FDateTime::Now().GetSecond());

    ChatHistory.Add(Msg);
    if (ChatHistory.Num() > MAX_CHAT_HISTORY)
        ChatHistory.RemoveAt(0);

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
            FString::Printf(TEXT("[He thong] %s"), *Message));
}

// === PARTY ===

bool UVNOSocialSubsystem::CreateParty()
{
    if (IsInParty())
    {
        AddSystemMessage(TEXT("Ban da co trong party!"));
        return false;
    }

    // Get player name
    FString LeaderName = TEXT("Ban");
    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            if (AVNOPlayerState* PS = PC->GetPlayerState<AVNOPlayerState>())
                LeaderName = PS->DisplayName;
        }
    }

    CurrentParty.PartyId = FString::Printf(TEXT("PARTY-%s"), *FDateTime::Now().ToString(TEXT("%Y%m%d-%H%M%S")));
    CurrentParty.LeaderName = LeaderName;

    FVNOPlayerInfo Self;
    Self.PlayerId = TEXT("self");
    Self.DisplayName = LeaderName;
    Self.bInParty = true;
    CurrentParty.Members.Add(Self);
    CurrentParty.MemberCount = 1;

    AddSystemMessage(FString::Printf(TEXT("Da tao party! ID: %s"), *CurrentParty.PartyId));
    return true;
}

bool UVNOSocialSubsystem::JoinParty(const FString& PartyId)
{
    if (IsInParty())
    {
        AddSystemMessage(TEXT("Ban da co trong party — roi party truoc!"));
        return false;
    }

    CurrentParty.PartyId = PartyId;
    CurrentParty.LeaderName = TEXT("Dong doi");

    FVNOPlayerInfo Self;
    Self.PlayerId = TEXT("self");
    Self.DisplayName = TEXT("Ban");
    Self.bInParty = true;
    CurrentParty.Members.Add(Self);
    CurrentParty.MemberCount = 1;

    AddSystemMessage(FString::Printf(TEXT("Da tham gia party: %s"), *PartyId));
    return true;
}

bool UVNOSocialSubsystem::LeaveParty()
{
    if (!IsInParty())
    {
        AddSystemMessage(TEXT("Ban khong o trong party nao!"));
        return false;
    }

    CurrentParty = FVNOPartyInfo();
    AddSystemMessage(TEXT("Da roi party."));
    return true;
}

// === FRIENDS ===

bool UVNOSocialSubsystem::AddFriend(const FString& PlayerId, const FString& DisplayName)
{
    // Check if already friends
    for (const FVNOPlayerInfo& F : Friends)
    {
        if (F.PlayerId == PlayerId)
        {
            AddSystemMessage(FString::Printf(TEXT("%s da la ban be!"), *DisplayName));
            return false;
        }
    }

    FVNOPlayerInfo NewFriend;
    NewFriend.PlayerId = PlayerId;
    NewFriend.DisplayName = DisplayName;
    NewFriend.bIsFriend = true;
    Friends.Add(NewFriend);

    AddSystemMessage(FString::Printf(TEXT("Da them %s vao danh sach ban!"), *DisplayName));
    return true;
}

bool UVNOSocialSubsystem::RemoveFriend(const FString& PlayerId)
{
    for (int32 i = 0; i < Friends.Num(); ++i)
    {
        if (Friends[i].PlayerId == PlayerId)
        {
            AddSystemMessage(FString::Printf(TEXT("Da xoa %s khoi ban be!"), *Friends[i].DisplayName));
            Friends.RemoveAt(i);
            return true;
        }
    }
    return false;
}

bool UVNOSocialSubsystem::IsFriend(const FString& PlayerId) const
{
    for (const FVNOPlayerInfo& F : Friends)
    {
        if (F.PlayerId == PlayerId)
            return true;
    }
    return false;
}

void UVNOSocialSubsystem::UpdateOnlinePlayers(const TArray<FVNOPlayerInfo>& Players)
{
    OnlinePlayers = Players;
}
