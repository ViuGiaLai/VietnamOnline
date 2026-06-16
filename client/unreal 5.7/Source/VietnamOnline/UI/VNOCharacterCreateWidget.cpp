#include "UI/VNOCharacterCreateWidget.h"
#include "Player/VNOPlayerState.h"
#include "Player/VNOPlayerCharacter.h"
#include "SaveGame/VNOSupabaseSubsystem.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

namespace
{
    FString GenderLabel(int32 Choice)
    {
        return (Choice == 2) ? TEXT("Nu") : TEXT("Nam");
    }

    FString SkinLabel(int32 Choice)
    {
        if (Choice == 1) return TEXT("Sang");
        if (Choice == 3) return TEXT("Dam");
        return TEXT("Trung binh");
    }
}

void UVNOCharacterCreateWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!WidgetTree) return;

    UVerticalBox* Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Root"));
    WidgetTree->RootWidget = Root;

    HintText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Hint"));
    HintText->SetText(FText::FromString(TEXT("TAO NHAN VAT (FR-02)\n1/2 = Gioi tinh | Q/W = Tone da | Enter = Xac nhan")));
    Root->AddChild(HintText);

    NameInput = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass(), TEXT("NameInput"));
    NameInput->SetHintText(FText::FromString(TEXT("Ten nhan vat (2-32 ky tu)")));
    if (UVerticalBoxSlot* VSlot = Root->AddChildToVerticalBox(NameInput))
    {
        VSlot->SetPadding(FMargin(0, 12, 0, 12));
    }

    UButton* ConfirmBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ConfirmBtn"));
    UTextBlock* BtnLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("BtnLabel"));
    BtnLabel->SetText(FText::FromString(TEXT("Bat dau Hoi An")));
    ConfirmBtn->AddChild(BtnLabel);
    ConfirmBtn->OnClicked.AddDynamic(this, &UVNOCharacterCreateWidget::ConfirmCreation);
    Root->AddChild(ConfirmBtn);

    CycleGender();
    CycleSkinTone();
}

void UVNOCharacterCreateWidget::CycleGender()
{
    GenderChoice = (GenderChoice == 1) ? 2 : 1;
    if (HintText)
    {
        HintText->SetText(FText::FromString(FString::Printf(
            TEXT("TAO NHAN VAT (FR-02)\nGioi tinh: %s | Tone da: %s\n1/2 doi gioi tinh | Q/W doi tone | Enter xac nhan"),
            *GenderLabel(GenderChoice), *SkinLabel(SkinChoice))));
    }
}

void UVNOCharacterCreateWidget::SetGenderChoice(int32 Choice)
{
    GenderChoice = FMath::Clamp(Choice, 1, 2);
    if (HintText)
    {
        HintText->SetText(FText::FromString(FString::Printf(
            TEXT("TAO NHAN VAT (FR-02)\nGioi tinh: %s | Tone da: %s\n1/2 doi gioi tinh | Q/W doi tone | Enter xac nhan"),
            *GenderLabel(GenderChoice), *SkinLabel(SkinChoice))));
    }
}

void UVNOCharacterCreateWidget::CycleSkinTone()
{
    AdjustSkinTone(1);
}

void UVNOCharacterCreateWidget::AdjustSkinTone(int32 Delta)
{
    SkinChoice = FMath::Clamp(SkinChoice + Delta, 1, 3);
    if (HintText)
    {
        HintText->SetText(FText::FromString(FString::Printf(
            TEXT("TAO NHAN VAT (FR-02)\nGioi tinh: %s | Tone da: %s\n1/2 doi gioi tinh | Q/W doi tone | Enter xac nhan"),
            *GenderLabel(GenderChoice), *SkinLabel(SkinChoice))));
    }
}

void UVNOCharacterCreateWidget::ConfirmCreation()
{
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    AVNOPlayerState* PS = PC->GetPlayerState<AVNOPlayerState>();
    AVNOPlayerCharacter* Ch = Cast<AVNOPlayerCharacter>(PC->GetPawn());
    if (!PS || !Ch) return;

    FString Name = NameInput ? NameInput->GetText().ToString().TrimStartAndEnd() : TEXT("Du khach");
    if (Name.Len() < 2) Name = TEXT("Du khach");
    if (Name.Len() > 32) Name = Name.Left(32);

    PS->DisplayName = Name;
    PS->Gender = (GenderChoice == 2) ? TEXT("nu") : TEXT("nam");
    PS->SkinTone = (SkinChoice == 1) ? TEXT("light") : (SkinChoice == 3) ? TEXT("dark") : TEXT("medium");
    PS->bCharacterCreated = true;
    PS->VndBalance = Ch->GetVndBalance();

    Ch->ApplyAppearanceFromState(PS);
    SetVisibility(ESlateVisibility::Collapsed);
    PC->SetShowMouseCursor(false);
    PC->SetInputMode(FInputModeGameOnly());

    if (UVNOSupabaseSubsystem* Supa = GetGameInstance()->GetSubsystem<UVNOSupabaseSubsystem>())
    {
        Supa->SaveCharacterProfile(PS, Ch);
    }
    UE_LOG(LogTemp, Log, TEXT("[FR-02] Character created: %s (%s, %s)"), *Name, *PS->Gender, *PS->SkinTone);
}
