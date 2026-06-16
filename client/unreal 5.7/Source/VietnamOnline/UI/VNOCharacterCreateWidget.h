#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VNOCharacterCreateWidget.generated.h"

class UEditableTextBox;
class UButton;
class UTextBlock;

/** FR-02 — Character creation (gender, skin tone, display name) */
UCLASS()
class VIETNAMONLINE_API UVNOCharacterCreateWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "VNO|Character")
    void ConfirmCreation();

    UFUNCTION(BlueprintCallable, Category = "VNO|Character")
    void CycleGender();

    UFUNCTION(BlueprintCallable, Category = "VNO|Character")
    void CycleSkinTone();

    UFUNCTION(BlueprintCallable, Category = "VNO|Character")
    void SetGenderChoice(int32 Choice);

    UFUNCTION(BlueprintCallable, Category = "VNO|Character")
    void AdjustSkinTone(int32 Delta);

protected:

    UPROPERTY()
    TObjectPtr<UTextBlock> HintText;

    UPROPERTY()
    TObjectPtr<UEditableTextBox> NameInput;

    int32 GenderChoice = 1;
    int32 SkinChoice = 2;
};
