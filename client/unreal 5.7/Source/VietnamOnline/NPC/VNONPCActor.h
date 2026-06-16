#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "vno/schemas/NPC.hpp"
#include "vno/enums/WorldEnums.hpp"
#include "VNONPCActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class AVNOPlayerCharacter;

UCLASS()
class VIETNAMONLINE_API AVNONPCActor : public AActor
{
    GENERATED_BODY()

public:
    AVNONPCActor();

    void InitFromData(const vno::NPC& NpcData);

    UFUNCTION(BlueprintCallable, Category = "VNO|NPC")
    bool TalkTo(AVNOPlayerCharacter* Player);

    UFUNCTION(BlueprintPure, Category = "VNO|NPC")
    FString GetNpcName() const { return NpcName; }

    UFUNCTION(BlueprintPure, Category = "VNO|NPC")
    FString GetNpcJob() const { return NpcJob; }

    UFUNCTION(BlueprintPure, Category = "VNO|NPC")
    FString GetNpcId() const { return NpcId; }

    UFUNCTION(BlueprintPure, Category = "VNO|NPC")
    int32 GetDialogueLineCount() const { return CurrentDialogue.Num(); }

    UFUNCTION(BlueprintPure, Category = "VNO|NPC")
    TArray<FString> GetDialogueLines() const { return CurrentDialogue; }

    UFUNCTION(BlueprintPure, Category = "VNO|NPC")
    bool HasBeenTalkedTo() const { return bTalkedTo; }

    void MarkTalkedTo() { bTalkedTo = true; }

protected:
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> NpcMesh;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UTextRenderComponent> NameLabel;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UTextRenderComponent> JobLabel;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USphereComponent> TalkRadius;

    UPROPERTY()
    TArray<FString> CurrentDialogue;

    UPROPERTY()
    TArray<FString> GreetDialogue;

    UPROPERTY()
    TArray<FString> BusyDialogue;

    UPROPERTY()
    TArray<FString> KnowledgeTags;

    FString NpcName;
    FString NpcJob;
    FString NpcId;
    FString RelatedPoiId;

    bool bTalkedTo = false;
    bool bMoodHappy = true;
};
