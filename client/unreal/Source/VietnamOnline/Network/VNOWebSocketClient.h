// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Common/UdpSocketBuilder.h"
#include "Common/UdpSocketReceiver.h"
#include "Networking.h"
#include "VNOWebSocketClient.generated.h"

/**
 * WebSocket client for connecting to Vietnam Online game server
 * Doc: FR-04 Multiplayer, N.1 Server Communication
 */
UCLASS(Blueprintable, BlueprintType)
class VIETNAMONLINE_API UVNOWebSocketClient : public UObject
{
    GENERATED_BODY()

public:
    UVNOWebSocketClient();
    virtual ~UVNOWebSocketClient();

    // Connect to server
    UFUNCTION(BlueprintCallable, Category = "VNO|Multiplayer")
    bool Connect(const FString& ServerUrl, int32 Port);

    // Disconnect from server
    UFUNCTION(BlueprintCallable, Category = "VNO|Multiplayer")
    void Disconnect();

    // Send position update to server
    UFUNCTION(BlueprintCallable, Category = "VNO|Multiplayer")
    void SendPositionUpdate(float X, float Y, float Z, float Yaw, int8 State, int8 VehicleType);

    // Send chat message
    UFUNCTION(BlueprintCallable, Category = "VNO|Multiplayer")
    void SendChatMessage(const FString& Message);

    // Send authentication
    UFUNCTION(BlueprintCallable, Category = "VNO|Multiplayer")
    void SendAuth(const FString& PlayerId, const FString& AccessToken);

    // Is connected
    UFUNCTION(BlueprintPure, Category = "VNO|Multiplayer")
    bool IsConnected() const { return bIsConnected; }

    // Get connected player count
    UFUNCTION(BlueprintPure, Category = "VNO|Multiplayer")
    int32 GetPlayerCount() const { return PlayerCount; }

    // Tick
    virtual void Tick(float DeltaTime);

    // Delegates
    FOnPlayerJoinedMulticastDelegate& OnPlayerJoined() { return PlayerJoinedDelegate; }
    FOnPlayerLeftMulticastDelegate& OnPlayerLeft() { return PlayerLeftDelegate; }
    FOnChatReceivedMulticastDelegate& OnChatReceived() { return ChatReceivedDelegate; }
    FOnPositionUpdatedMulticastDelegate& OnPositionUpdated() { return PositionUpdatedDelegate; }

protected:
    // Called when connected
    virtual void OnConnected();

    // Called when disconnected
    virtual void OnDisconnected();

    // Called when message received
    virtual void OnMessageReceived(const FString& Message);

    // Parse incoming message
    void ParseMessage(const FString& Json);

    // Create WebSocket frame
    FString CreateWebSocketFrame(const FString& Message);

    // Parse WebSocket frame
    FString ParseWebSocketFrame(const TArray<uint8>& Data);

private:
    // Socket
    TSharedPtr<FSocket> Socket;
    TSharedPtr<FInternetAddr> ServerAddr;

    // Connection state
    bool bIsConnected = false;
    int32 PlayerCount = 0;

    // Server info
    FString ServerUrl;
    int32 ServerPort;

    // Player ID
    FString PlayerId;
    FString AccessToken;

    // Last position for interpolation
    FVector LastPosition;
    FRotator LastRotation;
    int8 LastState = 0;
    int8 LastVehicleType = 0;

    // Receive buffer
    TArray<uint8> ReceiveBuffer;

    // Delegates
    FOnPlayerJoinedMulticastDelegate PlayerJoinedDelegate;
    FOnPlayerLeftMulticastDelegate PlayerLeftDelegate;
    FOnChatReceivedMulticastDelegate ChatReceivedDelegate;
    FOnPositionUpdatedMulticastDelegate PositionUpdatedDelegate;

    // Heartbeat
    float HeartbeatTimer = 0.0f;
    static constexpr float HEARTBEAT_INTERVAL = 5.0f;
};

// Inline implementation

inline UVNOWebSocketClient::UVNOWebSocketClient()
{
    if (HasAnyFlags(RF_ClassDefaultObject) == false)
    {
        // Don't tick by default
        bTickIsEnabled = false;
    }
}

inline UVNOWebSocketClient::~UVNOWebSocketClient()
{
    Disconnect();
}

inline void UVNOWebSocketClient::Tick(float DeltaTime)
{
    if (!bIsConnected || !Socket.IsValid())
    {
        return;
    }

    // Send heartbeat
    HeartbeatTimer += DeltaTime;
    if (HeartbeatTimer >= HEARTBEAT_INTERVAL)
    {
        HeartbeatTimer = 0.0f;
        // TODO: Send ping
    }

    // Receive data
    uint8 Buffer[4096];
    int32 BytesRead = 0;

    if (Socket->Recv(Buffer, sizeof(Buffer), BytesRead))
    {
        if (BytesRead > 0)
        {
            ReceiveBuffer.Append(Buffer, BytesRead);
            ParseWebSocketFrame(ReceiveBuffer);
        }
    }
}

inline void UVNOWebSocketClient::OnConnected()
{
    UE_LOG(LogTemp, Log, TEXT("[VNO] WebSocket connected to %s:%d"), *ServerUrl, ServerPort);
    bIsConnected = true;
    EnableTick();
}

inline void UVNOWebSocketClient::OnDisconnected()
{
    UE_LOG(LogTemp, Log, TEXT("[VNO] WebSocket disconnected"));
    bIsConnected = false;
    DisableTick();
}

inline void UVNOWebSocketClient::OnMessageReceived(const FString& Message)
{
    ParseMessage(Message);
}

inline FString UVNOWebSocketClient::CreateWebSocketFrame(const FString& Message)
{
    // Simple WebSocket TEXT frame
    FString Frame;
    Frame.Reserve(Message.Len() + 10);

    // FIN + TEXT opcode (0x81)
    Frame.AppendChar(0x81);

    // Payload length
    if (Message.Len() < 126)
    {
        Frame.AppendChar(static_cast<TCHAR>(Message.Len()));
    }
    else
    {
        Frame.AppendChar(126);
        Frame.AppendChar(static_cast<TCHAR>((Message.Len() >> 8) & 0xFF)));
        Frame.AppendChar(static_cast<TCHAR>(Message.Len() & 0xFF));
    }

    Frame.Append(Message);
    return Frame;
}

inline FString UVNOWebSocketClient::ParseWebSocketFrame(const TArray<uint8>& Data)
{
    if (Data.Num() < 2) return "";

    uint8 FirstByte = Data[0];
    uint8 Opcode = FirstByte & 0x0F;

    // Handle close
    if (Opcode == 0x08)
    {
        Disconnect();
        return "";
    }

    // Only handle TEXT
    if (Opcode != 0x01 && Opcode != 0x00)
    {
        return "";
    }

    uint8 SecondByte = Data[1];
    bool bMasked = (SecondByte & 0x80) != 0;

    size_t PayloadLength = SecondByte & 0x7F;
    size_t Offset = 2;

    if (PayloadLength == 126)
    {
        if (Data.Num() < 4) return "";
        PayloadLength = (Data[2] << 8) | Data[3];
        Offset = 4;
    }
    else if (PayloadLength == 127)
    {
        Offset = 10;
        PayloadLength = 0;
    }

    if (Data.Num() < Offset + PayloadLength) return "";

    FString Payload;
    Payload.AppendBytes(Data.GetData() + Offset, PayloadLength);

    // Unmask if needed
    if (bMasked && Payload.Len() >= 4)
    {
        uint8 Mask[4];
        FMemory::Memcpy(Mask, Payload.GetData(), 4);

        for (int32 i = 0; i < Payload.Len() - 4; ++i)
        {
            Payload.GetCharArray()[i + 4] ^= Mask[i % 4];
        }
    }

    return Payload;
}

} // namespace UE