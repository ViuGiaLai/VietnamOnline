#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VNOPlayerController.generated.h"

class UVNOCharacterCreateWidget;

UCLASS()
class VIETNAMONLINE_API AVNOPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    UFUNCTION(Exec)
    void VnoLogin(const FString& Email, const FString& Password);

    UFUNCTION(Exec)
    void VnoSignUp(const FString& Email, const FString& Password);

protected:
    void StartAuthAndCharacterFlow();
    void ShowCharacterCreationUI();
    void OnAuthDone(bool bSuccess, const FString& Message);
    void OnSaveDone(bool bSuccess, const FString& Message);
    void TriggerAutoSave();

    void OnCharCreateGenderMale();
    void OnCharCreateGenderFemale();
    void OnCharCreateSkinDown();
    void OnCharCreateSkinUp();
    void OnCharCreateConfirm();

    UPROPERTY()
    TObjectPtr<UVNOCharacterCreateWidget> CharacterCreateWidget;

    FTimerHandle AutoSaveTimer;
};
