#include "Network/VNOWebSocketClient.h"
#include "Engine/Engine.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

bool UVNOWebSocketClient::ConnectToServer(const FString& InServerUrl, int32 Port)
{
    ServerUrl = InServerUrl;
    ServerPort = Port;

    UE_LOG(LogTemp, Log, TEXT("[WS] Connecting to %s:%d..."), *ServerUrl, ServerPort);

    // In a full implementation: create TCP socket + WebSocket upgrade handshake
    // For MVP: simulate connection
    bIsConnected = true;
    OnConnected();

    // Send auth if we have credentials
    if (!PlayerId.IsEmpty() && !AccessToken.IsEmpty())
    {
        SendAuth(PlayerId, AccessToken);
    }

    return true;
}

void UVNOWebSocketClient::SendChatMessage(const FString& Message)
{
    if (!bIsConnected)
    {
        UE_LOG(LogTemp, Warning, TEXT("[WS] Cannot send chat — not connected"));
        return;
    }

    // Build JSON frame
    TSharedPtr<FJsonObject> Frame = MakeShared<FJsonObject>();
    Frame->SetStringField(TEXT("type"), TEXT("chat"));
    Frame->SetStringField(TEXT("sender"), PlayerId);

    // Try to parse message as JSON, or send as-is
    TSharedPtr<FJsonObject> MsgJson;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);
    if (FJsonSerializer::Deserialize(Reader, MsgJson) && MsgJson.IsValid())
    {
        Frame->SetObjectField(TEXT("data"), MsgJson);
    }
    else
    {
        Frame->SetStringField(TEXT("data"), Message);
    }

    FString Output;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
    FJsonSerializer::Serialize(Frame.ToSharedRef(), Writer);

    // In full implementation: send via TCP socket
    // For MVP: echo back as server response (simulate receiving from other player)
    FString EchoSender = PlayerId + TEXT(" (you)");
    ChatReceivedDelegate.Broadcast(EchoSender, Message);

    UE_LOG(LogTemp, Verbose, TEXT("[WS] Chat sent: %s"), *Output);
}

// Connect() is defined inline in the header — do not duplicate here

void UVNOWebSocketClient::ParseMessage(const FString& Json)
{
    TSharedPtr<FJsonObject> Root;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[WS] Failed to parse message JSON"));
        return;
    }

    FString Type;
    if (!Root->TryGetStringField(TEXT("type"), Type))
    {
        UE_LOG(LogTemp, Warning, TEXT("[WS] Message missing 'type' field"));
        return;
    }

    if (Type == TEXT("chat"))
    {
        FString Sender;
        FString Content;

        Root->TryGetStringField(TEXT("sender"), Sender);
        if (Sender.IsEmpty()) Sender = TEXT("Unknown");

        const TSharedPtr<FJsonObject>* DataObj = nullptr;
        if (Root->TryGetObjectField(TEXT("data"), DataObj))
        {
            (*DataObj)->TryGetStringField(TEXT("content"), Content);
            if (Content.IsEmpty())
                (*DataObj)->TryGetStringField(TEXT("text"), Content);
        }

        if (Content.IsEmpty())
            Root->TryGetStringField(TEXT("content"), Content);

        if (!Content.IsEmpty())
        {
            ChatReceivedDelegate.Broadcast(Sender, Content);
        }
    }
    else if (Type == TEXT("join"))
    {
        FString PlayerName;
        Root->TryGetStringField(TEXT("name"), PlayerName);
        if (PlayerName.IsEmpty())
            Root->TryGetStringField(TEXT("sender"), PlayerName);
        if (!PlayerName.IsEmpty())
            PlayerJoinedDelegate.Broadcast(PlayerName);
    }
    else if (Type == TEXT("leave"))
    {
        FString PlayerName;
        Root->TryGetStringField(TEXT("name"), PlayerName);
        if (PlayerName.IsEmpty())
            Root->TryGetStringField(TEXT("sender"), PlayerName);
        if (!PlayerName.IsEmpty())
            PlayerLeftDelegate.Broadcast(PlayerName);
    }
    else if (Type == TEXT("position"))
    {
        FString PId;
        double X = 0, Y = 0, Z = 0;
        float Yaw = 0;
        uint8 State = 0;

        Root->TryGetStringField(TEXT("player_id"), PId);
        const TSharedPtr<FJsonObject>* PosObj = nullptr;
        if (Root->TryGetObjectField(TEXT("position"), PosObj))
        {
            (*PosObj)->TryGetNumberField(TEXT("x"), X);
            (*PosObj)->TryGetNumberField(TEXT("y"), Y);
            (*PosObj)->TryGetNumberField(TEXT("z"), Z);
        }
        Root->TryGetNumberField(TEXT("rotation"), Yaw);
        Root->TryGetNumberField(TEXT("state"), State);

        if (!PId.IsEmpty())
        {
            PositionUpdatedDelegate.Broadcast(PId,
                static_cast<float>(X), static_cast<float>(Y), static_cast<float>(Z),
                Yaw, State);
        }
    }
}
