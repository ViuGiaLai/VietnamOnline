#include "Player/VNOPlayerController.h"
#include "Core/VNOGameInstance.h"
#include "Core/VNOEnvSettings.h"
#include "InputCoreTypes.h"
#include "Player/VNOPlayerState.h"
#include "Player/VNOPlayerCharacter.h"
#include "SaveGame/VNOSupabaseSubsystem.h"
#include "UI/VNOCharacterCreateWidget.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"

void AVNOPlayerController::BeginPlay()
{
    Super::BeginPlay();
    if (UVNOGameInstance* GI = Cast<UVNOGameInstance>(GetGameInstance()))
    {
        UE_LOG(LogTemp, Log, TEXT("[VNO] PlayerController — zone %s"), *GI->GetZoneId());
    }
    StartAuthAndCharacterFlow();

    int32 SaveSec = 30;
    if (UVNOGameInstance* GI = Cast<UVNOGameInstance>(GetGameInstance()))
    {
        if (UVNOEnvSettings* Env = GI->GetEnv())
            SaveSec = FMath::Max(5, FMath::RoundToInt(Env->GetFloat(TEXT("VNO_CHECKPOINT_SAVE_SEC"), 30.f)));
    }
    GetWorldTimerManager().SetTimer(AutoSaveTimer, this, &AVNOPlayerController::TriggerAutoSave, static_cast<float>(SaveSec), true);
}

void AVNOPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (EndPlayReason == EEndPlayReason::Quit || EndPlayReason == EEndPlayReason::EndPlayInEditor)
    {
        if (UVNOSupabaseSubsystem* Supa = GetGameInstance()->GetSubsystem<UVNOSupabaseSubsystem>())
        {
            Supa->SaveOnQuit();
        }
    }
    Super::EndPlay(EndPlayReason);
}

void AVNOPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    if (!InputComponent) return;

    InputComponent->BindKey(EKeys::One, IE_Pressed, this, &AVNOPlayerController::OnCharCreateGenderMale);
    InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AVNOPlayerController::OnCharCreateGenderFemale);
    InputComponent->BindKey(EKeys::Q, IE_Pressed, this, &AVNOPlayerController::OnCharCreateSkinDown);
    InputComponent->BindKey(EKeys::W, IE_Pressed, this, &AVNOPlayerController::OnCharCreateSkinUp);
    InputComponent->BindKey(EKeys::Enter, IE_Pressed, this, &AVNOPlayerController::OnCharCreateConfirm);
}

void AVNOPlayerController::StartAuthAndCharacterFlow()
{
    UVNOSupabaseSubsystem* Supa = GetGameInstance()->GetSubsystem<UVNOSupabaseSubsystem>();
    AVNOPlayerState* PS = GetPlayerState<AVNOPlayerState>();
    if (!Supa || !PS) return;

    Supa->OnAuthCompleted.AddDynamic(this, &AVNOPlayerController::OnAuthDone);
    Supa->OnSaveCompleted.AddDynamic(this, &AVNOPlayerController::OnSaveDone);

    if (Supa->DevSkipAuth())
    {
        PS->UserId = TEXT("DEV-LOCAL");
        PS->AccessToken = TEXT("dev");
        if (!PS->IsCharacterReady()) ShowCharacterCreationUI();
        else if (AVNOPlayerCharacter* Ch = Cast<AVNOPlayerCharacter>(GetPawn()))
            Supa->LoadCharacterProfile(PS, Ch);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[FR-01] Console: vno.Login email password | vno.SignUp email password"));
}

void AVNOPlayerController::VnoLogin(const FString& Email, const FString& Password)
{
    if (UVNOSupabaseSubsystem* Supa = GetGameInstance()->GetSubsystem<UVNOSupabaseSubsystem>())
        Supa->SignIn(Email, Password);
}

void AVNOPlayerController::VnoSignUp(const FString& Email, const FString& Password)
{
    if (UVNOSupabaseSubsystem* Supa = GetGameInstance()->GetSubsystem<UVNOSupabaseSubsystem>())
        Supa->SignUp(Email, Password);
}

void AVNOPlayerController::OnAuthDone(bool bSuccess, const FString& Message)
{
    if (!bSuccess)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Auth] %s"), *Message);
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("[Auth] OK user %s"), *Message);
    AVNOPlayerState* PS = GetPlayerState<AVNOPlayerState>();
    AVNOPlayerCharacter* Ch = Cast<AVNOPlayerCharacter>(GetPawn());
    if (PS && Ch)
    {
        if (UVNOSupabaseSubsystem* Supa = GetGameInstance()->GetSubsystem<UVNOSupabaseSubsystem>())
            Supa->LoadCharacterProfile(PS, Ch);
        if (!PS->IsCharacterReady()) ShowCharacterCreationUI();
    }
}

void AVNOPlayerController::OnSaveDone(bool bSuccess, const FString& Message)
{
    UE_LOG(LogTemp, Log, TEXT("[Save] %s — %s"), bSuccess ? TEXT("OK") : TEXT("FAIL"), *Message);
    if (bSuccess && Message.Contains(TEXT("Chua co save cloud")))
        ShowCharacterCreationUI();
}

void AVNOPlayerController::ShowCharacterCreationUI()
{
    if (CharacterCreateWidget) return;
    CharacterCreateWidget = CreateWidget<UVNOCharacterCreateWidget>(this, UVNOCharacterCreateWidget::StaticClass());
    if (CharacterCreateWidget)
    {
        CharacterCreateWidget->AddToViewport(100);
        bShowMouseCursor = true;
        SetInputMode(FInputModeGameAndUI());
    }
}

void AVNOPlayerController::TriggerAutoSave()
{
    AVNOPlayerState* PS = GetPlayerState<AVNOPlayerState>();
    AVNOPlayerCharacter* Ch = Cast<AVNOPlayerCharacter>(GetPawn());
    if (!PS || !Ch || !PS->IsCharacterReady()) return;
    PS->VndBalance = Ch->GetVndBalance();
    if (UVNOSupabaseSubsystem* Supa = GetGameInstance()->GetSubsystem<UVNOSupabaseSubsystem>())
        Supa->SaveCharacterProfile(PS, Ch);
}

void AVNOPlayerController::OnCharCreateGenderMale()
{
    if (CharacterCreateWidget && CharacterCreateWidget->IsVisible())
        CharacterCreateWidget->SetGenderChoice(1);
}

void AVNOPlayerController::OnCharCreateGenderFemale()
{
    if (CharacterCreateWidget && CharacterCreateWidget->IsVisible())
        CharacterCreateWidget->SetGenderChoice(2);
}

void AVNOPlayerController::OnCharCreateSkinDown()
{
    if (CharacterCreateWidget && CharacterCreateWidget->IsVisible())
        CharacterCreateWidget->AdjustSkinTone(-1);
}

void AVNOPlayerController::OnCharCreateSkinUp()
{
    if (CharacterCreateWidget && CharacterCreateWidget->IsVisible())
        CharacterCreateWidget->AdjustSkinTone(1);
}

void AVNOPlayerController::OnCharCreateConfirm()
{
    if (CharacterCreateWidget && CharacterCreateWidget->IsVisible())
        CharacterCreateWidget->ConfirmCreation();
}
