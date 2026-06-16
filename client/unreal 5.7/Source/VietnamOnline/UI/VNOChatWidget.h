#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "VNOChatWidget.generated.h"

/** FR-05: Chat input widget — type custom messages, select channel */
UCLASS()
class VIETNAMONLINE_API UVNOChatWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    void ShowChat();
    void HideChat();

protected:
    virtual FReply NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

    UFUNCTION()
    void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

    void CycleChannel();
    void SendCurrentMessage();

    UPROPERTY()
    TObjectPtr<UBorder> ChatBorder;

    UPROPERTY()
    TObjectPtr<UTextBlock> ChannelLabel;

    UPROPERTY()
    TObjectPtr<UEditableTextBox> ChatInput;

    TArray<FString> Channels;
    int32 CurrentChannelIndex = 0;
    bool bIsChatOpen = false;
};
