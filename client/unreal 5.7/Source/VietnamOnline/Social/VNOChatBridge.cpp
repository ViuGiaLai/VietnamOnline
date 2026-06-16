#include "Social/VNOChatBridge.h"
#include "Social/VNOSocialSubsystem.h"
#include "Network/VNOWebSocketClient.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

UVNOChatBridge::~UVNOChatBridge()
{
    SocialSubsystem = nullptr;
    WebSocket = nullptr;
}

void UVNOChatBridge::Initialize(UVNOSocialSubsystem* InSocial, UVNOWebSocketClient* InWS)
{
    SocialSubsystem = InSocial;
    WebSocket = InWS;

    if (!SocialSubsystem || !WebSocket)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ChatBridge] Missing Social or WebSocket reference"));
        return;
    }

    // Subscribe to WebSocket events
    WebSocket->OnChatReceived().AddUObject(this, &UVNOChatBridge::HandleChatReceived);
    WebSocket->OnPlayerJoined().AddUObject(this, &UVNOChatBridge::HandlePlayerJoined);
    WebSocket->OnPlayerLeft().AddUObject(this, &UVNOChatBridge::HandlePlayerLeft);

    UE_LOG(LogTemp, Log, TEXT("[ChatBridge] Initialized — Social <-> WebSocket bridge active"));
}

void UVNOChatBridge::SendChatToServer(const FString& Channel, const FString& Content)
{
    if (!WebSocket || !WebSocket->IsConnected())
    {
        // Offline: chat is local-only (already handled by SocialSubsystem)
        return;
    }

    // Build JSON message for server
    TSharedPtr<FJsonObject> MsgObj = MakeShared<FJsonObject>();
    MsgObj->SetStringField(TEXT("type"), TEXT("chat"));
    MsgObj->SetStringField(TEXT("channel"), Channel);
    MsgObj->SetStringField(TEXT("content"), Content);

    FString Output;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
    FJsonSerializer::Serialize(MsgObj.ToSharedRef(), Writer);

    WebSocket->SendChatMessage(Output);
    UE_LOG(LogTemp, Verbose, TEXT("[ChatBridge] Sent to server: %s"), *Output);
}

void UVNOChatBridge::OnPlayerJoined(const FString& PlayerName)
{
    HandlePlayerJoined(PlayerName);
}

void UVNOChatBridge::OnPlayerLeft(const FString& PlayerName)
{
    HandlePlayerLeft(PlayerName);
}

void UVNOChatBridge::HandleChatReceived(const FString& SenderName, const FString& Content)
{
    if (!SocialSubsystem) return;

    // Parse JSON if it's from server, or use raw content
    FString CleanSender = SenderName;
    FString CleanContent = Content;

    // Try to parse as JSON (server format)
    TSharedPtr<FJsonObject> Root;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);
    if (FJsonSerializer::Deserialize(Reader, Root) && Root.IsValid())
    {
        FString Type;
        if (Root->TryGetStringField(TEXT("type"), Type) && Type == TEXT("chat"))
        {
            FString Channel;
            Root->TryGetStringField(TEXT("channel"), Channel);
            Root->TryGetStringField(TEXT("content"), CleanContent);
            Root->TryGetStringField(TEXT("sender"), CleanSender);

            SocialSubsystem->SendChat(CleanContent, Channel.IsEmpty() ? TEXT("ZONE") : Channel);
            return;
        }
    }

    // Plain text fallback
    SocialSubsystem->SendChat(CleanContent, TEXT("ZONE"));
}

void UVNOChatBridge::HandlePlayerJoined(const FString& PlayerName)
{
    if (SocialSubsystem)
    {
        SocialSubsystem->AddSystemMessage(
            FString::Printf(TEXT("%s da vao Hoi An!"), *PlayerName));
    }
}

void UVNOChatBridge::HandlePlayerLeft(const FString& PlayerName)
{
    if (SocialSubsystem)
    {
        SocialSubsystem->AddSystemMessage(
            FString::Printf(TEXT("%s da roi khoi Hoi An."), *PlayerName));
    }
}
