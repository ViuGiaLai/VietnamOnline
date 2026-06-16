#include "Player/VNOPlayerCharacter.h"
#include "Player/VNOPlayerState.h"
#include "Player/VNOAnimInstance.h"
#include "POI/VNOPOIActor.h"
#include "Vehicles/VNOVehicleRentalStation.h"
#include "Food/VNOFoodShop.h"
#include "NPC/VNONPCActor.h"
#include "Quest/VNOQuestSubsystem.h"
#include "Weather/VNOWeatherSubsystem.h"
#include "Social/VNOSocialSubsystem.h"
#include "Social/VNOChatBridge.h"
#include "UI/VNOChatWidget.h"
#include "PhotoMode/VNOPhotoModeSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/UnrealNetwork.h"

AVNOPlayerCharacter::AVNOPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->JumpZVelocity = 420.f;
    GetCharacterMovement()->NavAgentProps.bCanWalk = true;
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 350.f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    SetupMannequinMesh();
}

void AVNOPlayerCharacter::ApplyBuff(const FString& BuffType, const FString& FoodName, float Value, int32 DurationSec)
{
    // Check for existing buff of same type → refresh duration
    for (int32 i = 0; i < ActiveBuffNames.Num(); ++i)
    {
        if (ActiveBuffNames[i] == BuffType)
        {
            BuffTimeRemaining[i] = static_cast<float>(DurationSec);
            UE_LOG(LogTemp, Log, TEXT("[Buff] Refresh %s: %.0f sec"), *BuffType, BuffTimeRemaining[i]);
            return;
        }
    }

    // New buff
    ActiveBuffNames.Add(BuffType);
    BuffTimeRemaining.Add(static_cast<float>(DurationSec));
    BuffValues.Add(Value);

    if (BuffType == TEXT("STAMINA_REGEN")) StaminaRegenMultiplier = 1.0f + Value;
    else if (BuffType == TEXT("SPEED_BOOST")) { SpeedMultiplier = 1.0f + Value; GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * SpeedMultiplier; }
    else if (BuffType == TEXT("EXP_BONUS")) ExpMultiplier = 1.0f + Value;
    else if (BuffType == TEXT("PRICE_DISCOUNT")) { /* handled at purchase */ }

    UE_LOG(LogTemp, Log, TEXT("[Buff] +%s: %.0f%% in %ds (from %s)"), *BuffType, Value * 100.f, DurationSec, *FoodName);
}

void AVNOPlayerCharacter::UpdateBuffs(float DeltaSeconds)
{
    for (int32 i = ActiveBuffNames.Num() - 1; i >= 0; --i)
    {
        BuffTimeRemaining[i] -= DeltaSeconds;
        if (BuffTimeRemaining[i] <= 0.f)
        {
            // Remove buff
            FString Type = ActiveBuffNames[i];
            if (Type == TEXT("STAMINA_REGEN")) StaminaRegenMultiplier = 1.0f;
            else if (Type == TEXT("SPEED_BOOST")) { SpeedMultiplier = 1.0f; GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; }
            else if (Type == TEXT("EXP_BONUS")) ExpMultiplier = 1.0f;

            ActiveBuffNames.RemoveAt(i);
            BuffTimeRemaining.RemoveAt(i);
            BuffValues.RemoveAt(i);
            UE_LOG(LogTemp, Log, TEXT("[Buff] Het: %s"), *Type);
        }
    }
}

void AVNOPlayerCharacter::SetupMannequinMesh()
{
    USkeletalMeshComponent* Mesh3P = GetMesh();
    Mesh3P->SetRelativeLocation(FVector(0.f, 0.f, -96.f));
    Mesh3P->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

    static const TCHAR* MeshPaths[] = {
        TEXT("/Game/VNO/Characters/Mesh/SKM_VNOPlayer.SKM_VNOPlayer"),
        TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"),
        TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple"),
    };
    for (const TCHAR* Path : MeshPaths)
    {
        if (USkeletalMesh* SK = LoadObject<USkeletalMesh>(nullptr, Path))
        {
            Mesh3P->SetSkeletalMesh(SK);
            break;
        }
    }

    static const TCHAR* AnimPaths[] = {
        TEXT("/Game/VNO/Characters/Anim/ABP_VNOPlayer.ABP_VNOPlayer_C"),
        TEXT("/Game/Characters/Mannequins/Animations/ABP_Manny_Simple.ABP_Manny_Simple_C"),
    };
    for (const TCHAR* Path : AnimPaths)
    {
        if (UClass* AnimClass = LoadClass<UAnimInstance>(nullptr, Path))
        {
            Mesh3P->SetAnimInstanceClass(AnimClass);
            return;
        }
    }
    Mesh3P->SetAnimInstanceClass(UVNOAnimInstance::StaticClass());
}

void AVNOPlayerCharacter::ApplyAppearanceFromState(AVNOPlayerState* InPlayerState)
{
    if (!InPlayerState) return;
    VndBalance = InPlayerState->VndBalance;


    if (USkeletalMeshComponent* Mesh3P = GetMesh())
    {
        const bool bFemale = InPlayerState->Gender == TEXT("nu");
        static const TCHAR* FemalePaths[] = {
            TEXT("/Game/VNO/Characters/Mesh/SKM_VNOPlayer_F.SKM_VNOPlayer_F"),
            TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple"),
        };
        static const TCHAR* MalePaths[] = {
            TEXT("/Game/VNO/Characters/Mesh/SKM_VNOPlayer.SKM_VNOPlayer"),
            TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"),
        };
        const TCHAR* const* Paths = bFemale ? FemalePaths : MalePaths;
        for (int32 i = 0; i < 2; ++i)
        {
            if (USkeletalMesh* SK = LoadObject<USkeletalMesh>(nullptr, Paths[i]))
            {
                Mesh3P->SetSkeletalMesh(SK);
                break;
            }
        }

        float Tone = 1.f;
        if (InPlayerState->SkinTone == TEXT("light")) Tone = 1.15f;
        else if (InPlayerState->SkinTone == TEXT("dark")) Tone = 0.75f;
        Mesh3P->SetVectorParameterValueOnMaterials(TEXT("SkinTone"), FVector(Tone, Tone, Tone));
    }
}

void AVNOPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    StaminaCurrent = StaminaMax;
}

void AVNOPlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Performance: throttle non-essential updates to ~6Hz
    ThrottleCounter++;
    if (ThrottleCounter < THROTTLE_SKIP) return;
    ThrottleCounter = 0;

    if (!bPhotoMode) UpdateStamina(DeltaSeconds * THROTTLE_SKIP);
    UpdateBuffs(DeltaSeconds * THROTTLE_SKIP);
}

float AVNOPlayerCharacter::GetStaminaPercent() const
{
    return StaminaMax > 0.f ? StaminaCurrent / StaminaMax : 0.f;
}

void AVNOPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AVNOPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AVNOPlayerCharacter::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AVNOPlayerCharacter::Turn);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AVNOPlayerCharacter::LookUp);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &AVNOPlayerCharacter::StartSprint);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &AVNOPlayerCharacter::StopSprint);
    PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AVNOPlayerCharacter::Interact);
    PlayerInputComponent->BindAction(TEXT("PhotoMode"), IE_Pressed, this, &AVNOPlayerCharacter::TogglePhotoMode);
    PlayerInputComponent->BindAction(TEXT("CapturePhoto"), IE_Pressed, this, &AVNOPlayerCharacter::CapturePhoto);
    PlayerInputComponent->BindAction(TEXT("ToggleMinimap"), IE_Pressed, this, &AVNOPlayerCharacter::ToggleMinimap);
    PlayerInputComponent->BindAction(TEXT("QuestJournal"), IE_Pressed, this, &AVNOPlayerCharacter::OpenQuestJournal);
    PlayerInputComponent->BindAction(TEXT("OpenChat"), IE_Pressed, this, &AVNOPlayerCharacter::OpenChat);
    PlayerInputComponent->BindAction(TEXT("OpenParty"), IE_Pressed, this, &AVNOPlayerCharacter::OpenParty);
    PlayerInputComponent->BindAction(TEXT("OpenFriends"), IE_Pressed, this, &AVNOPlayerCharacter::OpenFriends);
}

void AVNOPlayerCharacter::MoveForward(float Value)
{
    if (bPhotoMode || FMath::IsNearlyZero(Value)) return;
    AddMovementInput(GetActorForwardVector(), Value);
}

void AVNOPlayerCharacter::MoveRight(float Value)
{
    if (bPhotoMode || FMath::IsNearlyZero(Value)) return;
    AddMovementInput(GetActorRightVector(), Value);
}

void AVNOPlayerCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void AVNOPlayerCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AVNOPlayerCharacter::StartSprint()
{
    if (StaminaCurrent <= 0.f) return;
    bIsSprinting = true;
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AVNOPlayerCharacter::StopSprint()
{
    bIsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AVNOPlayerCharacter::UpdateStamina(float DeltaSeconds)
{
    // Weather modifier (FR-11): rain/hot weather reduces regen
    float WeatherMod = 1.0f;
    if (UWorld* World = GetWorld())
    {
        if (UVNOWeatherSubsystem* W = World->GetSubsystem<UVNOWeatherSubsystem>())
            WeatherMod = W->GetStaminaRegenModifier();
    }

    if (bIsSprinting && GetVelocity().SizeSquared() > 100.f)
    {
        // Sprint drain is also affected by weather (hot = drain faster)
        float DrainMod = (WeatherMod < 1.0f) ? (1.0f + (1.0f - WeatherMod)) : 1.0f;
        StaminaCurrent = FMath::Max(0.f, StaminaCurrent - 1.f * DrainMod * DeltaSeconds);
        if (StaminaCurrent <= 0.f) StopSprint();
    }
    else
    {
        // Regen is multiplied by weather modifier
        StaminaCurrent = FMath::Min(StaminaMax, StaminaCurrent + 2.f * WeatherMod * DeltaSeconds * StaminaRegenMultiplier);
    }
}

void AVNOPlayerCharacter::Interact()
{
    UWorld* World = GetWorld();
    if (!World) return;

    FVector Loc = GetActorLocation();

    // Try food shop first (closest within 400cm)
    AVNOFoodShop* NearestFood = nullptr;
    float NearestFoodDist = 400.f * 400.f;
    for (TActorIterator<AVNOFoodShop> It(World); It; ++It)
    {
        float D = FVector::DistSquared(Loc, It->GetActorLocation());
        if (D < NearestFoodDist)
        {
            NearestFoodDist = D;
            NearestFood = *It;
        }
    }
    if (NearestFood)
    {
        // Order first menu item
        for (int32 i = 0; i < NearestFood->GetMenu().Num(); ++i)
        {
            if (NearestFood->TryOrderFood(this, i))
                return;
        }
        return;
    }

    // Try NPC interaction (closest within 350cm)
    AVNONPCActor* NearestNPC = nullptr;
    float NearestNPCDist = 350.f * 350.f;
    for (TActorIterator<AVNONPCActor> It(World); It; ++It)
    {
        float D = FVector::DistSquared(Loc, It->GetActorLocation());
        if (D < NearestNPCDist)
        {
            NearestNPCDist = D;
            NearestNPC = *It;
        }
    }
    if (NearestNPC)
    {
        NearestNPC->TalkTo(this);
        return;
    }

    // Try vehicle rental station (within 400cm)
    AVNOVehicleRentalStation* NearestStation = nullptr;
    float NearestStationDist = 400.f * 400.f;
    for (TActorIterator<AVNOVehicleRentalStation> It(World); It; ++It)
    {
        float D = FVector::DistSquared(Loc, It->GetActorLocation());
        if (D < NearestStationDist)
        {
            NearestStationDist = D;
            NearestStation = *It;
        }
    }
    if (NearestStation)
    {
        // Try to rent first available vehicle
        for (int32 i = 0; i < NearestStation->GetVehicles().Num(); ++i)
        {
            if (NearestStation->TryRentVehicle(this, i))
                return;
        }
        return;
    }

    // Try POI check-in
    TArray<AActor*> Overlapping;
    GetOverlappingActors(Overlapping, AVNOPOIActor::StaticClass());
    float BestDist = TNumericLimits<float>::Max();
    AVNOPOIActor* BestPOI = nullptr;
    for (AActor* A : Overlapping)
    {
        float D = FVector::DistSquared(Loc, A->GetActorLocation());
        if (D < BestDist) { BestDist = D; BestPOI = Cast<AVNOPOIActor>(A); }
    }
    if (!BestPOI)
    {
        FHitResult Hit;
        FVector Start = Loc;
        FVector End = Start + GetActorForwardVector() * 300.f;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);
        if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
        {
            BestPOI = Cast<AVNOPOIActor>(Hit.GetActor());
        }
    }
    if (BestPOI) BestPOI->TryCheckIn(this);
}

void AVNOPlayerCharacter::TogglePhotoMode()
{
    UVNOPhotoModeSubsystem* Photo = GetWorld()->GetSubsystem<UVNOPhotoModeSubsystem>();
    if (!Photo) return;

    bPhotoMode = Photo->TogglePhotoMode(this);
}

void AVNOPlayerCharacter::CapturePhoto()
{
    if (!bPhotoMode) return;

    UVNOPhotoModeSubsystem* Photo = GetWorld()->GetSubsystem<UVNOPhotoModeSubsystem>();
    if (!Photo) return;

    Photo->CapturePhoto(this);
}

void AVNOPlayerCharacter::ToggleMinimap()
{
    bShowMinimap = !bShowMinimap;
}

void AVNOPlayerCharacter::OpenQuestJournal()
{
    if (UVNOQuestSubsystem* Q = GetWorld()->GetSubsystem<UVNOQuestSubsystem>())
    {
        Q->LogActiveQuests();
    }
}

void AVNOPlayerCharacter::OpenChat()
{
    if (bChatOpen)
    {
        // Close chat if already open
        if (ChatWidget)
            ChatWidget->HideChat();
        bChatOpen = false;
        return;
    }

    // Create widget if needed
    if (!ChatWidget)
    {
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            ChatWidget = CreateWidget<UVNOChatWidget>(PC, UVNOChatWidget::StaticClass());
            if (ChatWidget)
            {
                ChatWidget->AddToViewport(200);
            }
        }
    }

    if (ChatWidget)
    {
        ChatWidget->ShowChat();
        bChatOpen = true;
        UE_LOG(LogTemp, Log, TEXT("[Social] Chat widget opened"));
    }
}

void AVNOPlayerCharacter::OpenParty()
{
    UWorld* World = GetWorld();
    if (!World) return;

    UVNOSocialSubsystem* Social = World->GetSubsystem<UVNOSocialSubsystem>();
    if (!Social) return;

    if (Social->IsInParty())
    {
        Social->LeaveParty();
    }
    else
    {
        Social->CreateParty();
    }
}

void AVNOPlayerCharacter::OpenFriends()
{
    UWorld* World = GetWorld();
    if (!World) return;

    UVNOSocialSubsystem* Social = World->GetSubsystem<UVNOSocialSubsystem>();
    if (!Social) return;

    TArray<FVNOPlayerInfo> Friends = Social->GetFriends();
    if (Friends.Num() == 0)
    {
        Social->AddSystemMessage(TEXT("Ban chua co ban be nao. Moi ban bang: /friend add <PlayerId>"));
        // Add sample NPC friends for demo
        Social->AddFriend(TEXT("NPC-HDV-01"), TEXT("Anh Tuan"));
        Social->AddFriend(TEXT("NPC-BA-NAM"), TEXT("Ba Nam"));
    }
    else
    {
        FString FriendList;
        for (const FVNOPlayerInfo& F : Friends)
            FriendList += F.DisplayName + TEXT(", ");
        Social->AddSystemMessage(FString::Printf(TEXT("Ban be (%d): %s"), Friends.Num(), *FriendList));
    }
}

void AVNOPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AVNOPlayerCharacter, ReplicatedLocation);
    DOREPLIFETIME(AVNOPlayerCharacter, ReplicatedRotation);
    DOREPLIFETIME(AVNOPlayerCharacter, ReplicatedMovementState);
    DOREPLIFETIME(AVNOPlayerCharacter, ReplicatedVehicleType);
}

void AVNOPlayerCharacter::OnRep_ReplicatedMovement()
{
    SetActorLocation(ReplicatedLocation);
    SetActorRotation(ReplicatedRotation);
}

void AVNOPlayerCharacter::OnRep_MovementState()
{
    // Update movement based on replicated state
    UE_LOG(LogTemp, Log, TEXT("[VNO] Replicated state: %d"), ReplicatedMovementState);
}

void AVNOPlayerCharacter::OnRep_VehicleType()
{
    UE_LOG(LogTemp, Log, TEXT("[VNO] Replicated vehicle: %d"), ReplicatedVehicleType);
}

void AVNOPlayerCharacter::ConnectToServer(const FString& ServerUrl, int32 Port)
{
    if (!WebSocketClient)
    {
        WebSocketClient = NewObject<UVNOWebSocketClient>(this);
    }
    
    if (WebSocketClient->ConnectToServer(ServerUrl, Port))
    {
        bIsOnline = true;

        // Initialize chat bridge
        UVNOSocialSubsystem* Social = GetWorld()->GetSubsystem<UVNOSocialSubsystem>();
        if (Social && !ChatBridge)
        {
            ChatBridge = NewObject<UVNOChatBridge>(this);
            ChatBridge->Initialize(Social, WebSocketClient);
            UE_LOG(LogTemp, Log, TEXT("[VNO] Chat bridge initialized"));
        }

        UE_LOG(LogTemp, Log, TEXT("[VNO] Connected to server %s:%d"), *ServerUrl, Port);
    }
}

void AVNOPlayerCharacter::DisconnectFromServer()
{
    if (ChatBridge)
    {
        ChatBridge = nullptr;
    }
    if (WebSocketClient)
    {
        WebSocketClient->Disconnect();
    }
    bIsOnline = false;
    OtherPlayerCount = 0;
}

void AVNOPlayerCharacter::SendPositionToServer()
{
    if (!WebSocketClient || !bIsOnline) return;
    
    FVector Pos = GetActorLocation();
    FRotator Rot = GetActorRotation();
    
    WebSocketClient->SendPositionUpdate(
        Pos.X, Pos.Y, Pos.Z,
        Rot.Yaw,
        static_cast<uint8>(ReplicatedMovementState),
        static_cast<uint8>(ReplicatedVehicleType));
}

