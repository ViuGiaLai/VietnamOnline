#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Network/VNOWebSocketClient.h"
#include "VNOChatBridge.generated.h"

class UVNOSocialSubsystem;

/**
 * FR-05: Bridge between VNOSocialSubsystem and VNOWebSocketClient.
 * Routes local chat → WebSocket server, and incoming WS messages → social subsystem.
 */
UCLASS()
class VIETNAMONLINE_API UVNOChatBridge : public UObject
{
    GENERATED_BODY()

public:
    virtual ~UVNOChatBridge();

    void Initialize(class UVNOSocialSubsystem* InSocial, class UVNOWebSocketClient* InWS);

    void SendChatToServer(const FString& Channel, const FString& Content);

    void OnPlayerJoined(const FString& PlayerName);
    void OnPlayerLeft(const FString& PlayerName);

private:
    void HandleChatReceived(const FString& SenderName, const FString& Content);
    void HandlePlayerJoined(const FString& PlayerName);
    void HandlePlayerLeft(const FString& PlayerName);

    UPROPERTY()
    TObjectPtr<UVNOSocialSubsystem> SocialSubsystem;

    UPROPERTY()
    TObjectPtr<UVNOWebSocketClient> WebSocket;
};
