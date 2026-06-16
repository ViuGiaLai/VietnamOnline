#include "UI/VNOChatWidget.h"
#include "Social/VNOSocialSubsystem.h"
#include "Player/VNOPlayerCharacter.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Input/Events.h"
#include "InputCoreTypes.h"

void UVNOChatWidget::NativeConstruct()
{
    Super::NativeConstruct();

    Channels = { TEXT("ZONE"), TEXT("PARTY"), TEXT("WHISPER") };

    if (!WidgetTree) return;

    // Create root border
    ChatBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ChatBorder"));
    WidgetTree->RootWidget = ChatBorder;
    ChatBorder->SetVisibility(ESlateVisibility::Collapsed);
    ChatBorder->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.6f));
    ChatBorder->SetPadding(FMargin(8.f, 4.f));

    // Channel label
    ChannelLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ChannelLabel"));
    ChannelLabel->SetText(FText::FromString(TEXT("[ZONE]")));
    ChannelLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.6f, 0.8f, 1.f)));

    // Chat input
    ChatInput = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass(), TEXT("ChatInput"));
    ChatInput->SetHintText(FText::FromString(TEXT("Nhap tin nhan... Tab: doi channel | Enter: gui | Esc: thoat")));

    // Build layout: we need a VerticalBox or HorizontalBox
    UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VBox"));
    ChatBorder->SetContent(VBox);

    UHorizontalBox* HBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HBox"));
    HBox->AddChildToHorizontalBox(ChannelLabel);
    HBox->AddChildToHorizontalBox(ChatInput);

    VBox->AddChildToVerticalBox(HBox);

    // Bind events
    ChatInput->OnTextCommitted.AddDynamic(this, &UVNOChatWidget::OnTextCommitted);
}

void UVNOChatWidget::NativeDestruct()
{
    Super::NativeDestruct();
}

FReply UVNOChatWidget::NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
    FKey Key = InKeyEvent.GetKey();

    if (Key == EKeys::Escape)
    {
        HideChat();
        return FReply::Handled();
    }

    if (Key == EKeys::Tab)
    {
        CycleChannel();
        return FReply::Handled();
    }

    return Super::NativeOnKeyDown(MyGeometry, InKeyEvent);
}

void UVNOChatWidget::ShowChat()
{
    if (!ChatBorder || !ChatInput) return;

    bIsChatOpen = true;
    ChatBorder->SetVisibility(ESlateVisibility::Visible);
    ChatInput->SetUserFocus(GetOwningPlayer());
    ChatInput->SetKeyboardFocus();
    ChatInput->SetText(FText::GetEmpty());

    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->bShowMouseCursor = true;
        PC->SetInputMode(FInputModeGameAndUI());
    }
}

void UVNOChatWidget::HideChat()
{
    if (!ChatBorder) return;

    bIsChatOpen = false;
    ChatBorder->SetVisibility(ESlateVisibility::Collapsed);

    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());
    }
}

void UVNOChatWidget::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        SendCurrentMessage();
    }
    else if (CommitMethod == ETextCommit::OnCleared)
    {
        HideChat();
    }
}

void UVNOChatWidget::CycleChannel()
{
    CurrentChannelIndex = (CurrentChannelIndex + 1) % Channels.Num();
    if (ChannelLabel)
    {
        FString Chan = Channels[CurrentChannelIndex];
        FLinearColor Color;
        if (Chan == TEXT("PARTY")) Color = FLinearColor(0.f, 0.8f, 0.2f);
        else if (Chan == TEXT("WHISPER")) Color = FLinearColor(0.8f, 0.3f, 0.8f);
        else Color = FLinearColor(0.6f, 0.8f, 1.f);

        ChannelLabel->SetText(FText::FromString(FString::Printf(TEXT("[%s]"), *Chan)));
        ChannelLabel->SetColorAndOpacity(FSlateColor(Color));
    }
}

void UVNOChatWidget::SendCurrentMessage()
{
    if (!ChatInput) return;

    FString Content = ChatInput->GetText().ToString().TrimStartAndEnd();
    if (Content.IsEmpty())
    {
        HideChat();
        return;
    }

    FString Channel = Channels[CurrentChannelIndex];

    // Send via social subsystem
    if (UWorld* World = GetWorld())
    {
        if (UVNOSocialSubsystem* Social = World->GetSubsystem<UVNOSocialSubsystem>())
        {
            Social->SendChat(Content, Channel);
        }
    }

    // Log to console
    UE_LOG(LogTemp, Log, TEXT("[Chat][%s] %s"), *Channel, *Content);

    // Check for chat commands
    if (Content.StartsWith(TEXT("/")))
    {
        TArray<FString> Parts;
        Content.ParseIntoArray(Parts, TEXT(" "), true);
        if (Parts.Num() >= 1)
        {
            FString Cmd = Parts[0].ToLower();
            if (Cmd == TEXT("/party"))
            {
                if (UVNOSocialSubsystem* Social = GetWorld()->GetSubsystem<UVNOSocialSubsystem>())
                {
                    if (Social->IsInParty()) Social->LeaveParty();
                    else Social->CreateParty();
                }
            }
            else if (Cmd == TEXT("/friend") && Parts.Num() >= 3)
            {
                if (Parts[1] == TEXT("add"))
                {
                    // /friend add <PlayerId> <DisplayName>
                    FString FriendId = Parts[2];
                    FString FriendName = (Parts.Num() > 3) ? Parts[3] : FriendId;
                    if (UVNOSocialSubsystem* Social = GetWorld()->GetSubsystem<UVNOSocialSubsystem>())
                        Social->AddFriend(FriendId, FriendName);
                }
                else if (Parts[1] == TEXT("remove") && Parts.Num() >= 3)
                {
                    if (UVNOSocialSubsystem* Social = GetWorld()->GetSubsystem<UVNOSocialSubsystem>())
                        Social->RemoveFriend(Parts[2]);
                }
            }
            else if (Cmd == TEXT("/help"))
            {
                if (UVNOSocialSubsystem* Social = GetWorld()->GetSubsystem<UVNOSocialSubsystem>())
                {
                    Social->AddSystemMessage(TEXT("--- Chat Commands ---"));
                    Social->AddSystemMessage(TEXT("/party — Tao/Roi party"));
                    Social->AddSystemMessage(TEXT("/friend add <id> <ten> — Them ban"));
                    Social->AddSystemMessage(TEXT("/friend remove <id> — Xoa ban"));
                }
            }
        }
    }

    HideChat();
}
