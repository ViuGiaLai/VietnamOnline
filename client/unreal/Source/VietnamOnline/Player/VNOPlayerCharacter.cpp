#include "Player/VNOPlayerCharacter.h"
#include "Player/VNOPlayerState.h"
#include "Player/VNOAnimInstance.h"
#include "POI/VNOPOIActor.h"
#include "Quest/VNOQuestSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

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

void AVNOPlayerCharacter::ApplyAppearanceFromState(AVNOPlayerState* PlayerState)
{
    if (!PlayerState) return;
    VndBalance = PlayerState->VndBalance;

    if (USkeletalMeshComponent* Mesh3P = GetMesh())
    {
        const bool bFemale = PlayerState->Gender == TEXT("nu");
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
        if (PlayerState->SkinTone == TEXT("light")) Tone = 1.15f;
        else if (PlayerState->SkinTone == TEXT("dark")) Tone = 0.75f;
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
    if (!bPhotoMode) UpdateStamina(DeltaSeconds);
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
    PlayerInputComponent->BindAction(TEXT("ToggleMinimap"), IE_Pressed, this, &AVNOPlayerCharacter::ToggleMinimap);
    PlayerInputComponent->BindAction(TEXT("QuestJournal"), IE_Pressed, this, &AVNOPlayerCharacter::OpenQuestJournal);
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
    if (bIsSprinting && GetVelocity().SizeSquared() > 100.f)
    {
        StaminaCurrent = FMath::Max(0.f, StaminaCurrent - 1.f * DeltaSeconds);
        if (StaminaCurrent <= 0.f) StopSprint();
    }
    else
    {
        StaminaCurrent = FMath::Min(StaminaMax, StaminaCurrent + 2.f * DeltaSeconds);
    }
}

void AVNOPlayerCharacter::Interact()
{
    TArray<AActor*> Overlapping;
    GetOverlappingActors(Overlapping, AVNOPOIActor::StaticClass());
    float BestDist = TNumericLimits<float>::Max();
    AVNOPOIActor* Best = nullptr;
    for (AActor* A : Overlapping)
    {
        float D = FVector::DistSquared(GetActorLocation(), A->GetActorLocation());
        if (D < BestDist) { BestDist = D; Best = Cast<AVNOPOIActor>(A); }
    }
    if (!Best)
    {
        TArray<FHitResult> Hits;
        FVector Start = GetActorLocation();
        FVector End = Start + GetActorForwardVector() * 300.f;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);
        if (GetWorld()->LineTraceSingleByChannel(Hits, Start, End, ECC_Visibility, Params))
        {
            Best = Cast<AVNOPOIActor>(Hits[0].GetActor());
        }
    }
    if (Best) Best->TryCheckIn(this);
}

void AVNOPlayerCharacter::TogglePhotoMode()
{
    bPhotoMode = !bPhotoMode;
    if (bPhotoMode)
    {
        StopSprint();
        GetCharacterMovement()->DisableMovement();
        UE_LOG(LogTemp, Log, TEXT("[VNO] Photo Mode ON (F8)"));
    }
    else
    {
        GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    }
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
