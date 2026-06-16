#include "Multiplayer/VNOMultiplayerModule.h"
#include "Network/VNOWebSocketClient.h"
#include "Core/VNOGameInstance.h"
#include "vno/config/EnvConfig.hpp"

void FVNOMultiplayerModule::StartupModule() {
    UE_LOG(LogTemp, Log, TEXT("[VNO] Multiplayer Module starting..."));
    
    // Load config
    auto& env = vno::config::EnvConfig::instance();
    env.load("../../../.env");
    
    // Get server URL from config
    FString serverUrl = UTF8_TO_TCHAR(env.get("VNO_SERVER_WS_URL", "ws://localhost:8081").c_str());
    int32 serverPort = env.getInt("VNO_WEBSOCKET_PORT", 8081);
    
    UE_LOG(LogTemp, Log, TEXT("[VNO] Multiplayer configured: %s:%d"), *serverUrl, serverPort);
}

void FVNOMultiplayerModule::ShutdownModule() {
    UE_LOG(LogTemp, Log, TEXT("[VNO] Multiplayer Module stopped"));
}