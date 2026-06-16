#include "NPC/VNONPCActor.h"
#include "Player/VNOPlayerCharacter.h"
#include "Quest/VNOQuestSubsystem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "TimerManager.h"

AVNONPCActor::AVNONPCActor()
{
    PrimaryActorTick.bCanEverTick = false;

    TalkRadius = CreateDefaultSubobject<USphereComponent>(TEXT("TalkRadius"));
    RootComponent = TalkRadius;
    TalkRadius->SetSphereRadius(250.f);
    TalkRadius->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    TalkRadius->SetGenerateOverlapEvents(true);

    NpcMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NpcMesh"));
    NpcMesh->SetupAttachment(RootComponent);
    NpcMesh->SetRelativeLocation(FVector(0.f, 0.f, 45.f));
    NpcMesh->SetRelativeScale3D(FVector(0.7f, 0.7f, 0.9f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylFinder(
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (CylFinder.Succeeded())
        NpcMesh->SetStaticMesh(CylFinder.Object);

    NameLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NameLabel"));
    NameLabel->SetupAttachment(NpcMesh);
    NameLabel->SetRelativeLocation(FVector(0.f, 0.f, 95.f));
    NameLabel->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
    NameLabel->SetHorizontalAlignment(EHTA_Center);
    NameLabel->SetTextRenderColor(FColor::Yellow);
    NameLabel->SetWorldSize(32.f);

    JobLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("JobLabel"));
    JobLabel->SetupAttachment(NameLabel);
    JobLabel->SetRelativeLocation(FVector(0.f, 0.f, -32.f));
    JobLabel->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.6f));
    JobLabel->SetHorizontalAlignment(EHTA_Center);
    JobLabel->SetTextRenderColor(FColor(200, 200, 200));
    JobLabel->SetWorldSize(22.f);
}

void AVNONPCActor::InitFromData(const vno::NPC& NpcData)
{
    NpcId = UTF8_TO_TCHAR(NpcData.npc_id.c_str());
    NpcName = UTF8_TO_TCHAR(NpcData.name.c_str());
    NpcJob = UTF8_TO_TCHAR(NpcData.job.c_str());
    RelatedPoiId = UTF8_TO_TCHAR(NpcData.poi_id.c_str());

    bMoodHappy = (NpcData.mood == vno::NPCMood::HAPPY);

    // Copy dialogue
    for (const auto& Line : NpcData.dialogue_greet)
        GreetDialogue.Add(UTF8_TO_TCHAR(Line.c_str()));
    for (const auto& Line : NpcData.dialogue_busy)
        BusyDialogue.Add(UTF8_TO_TCHAR(Line.c_str()));
    for (const auto& Tag : NpcData.knowledge_tags)
        KnowledgeTags.Add(UTF8_TO_TCHAR(Tag.c_str()));

    // Set labels
    if (NameLabel)
        NameLabel->SetText(FText::FromString(NpcName));
    if (JobLabel)
        JobLabel->SetText(FText::FromString(NpcJob));

    // Set color based on mood
    if (NpcMesh)
    {
        if (UMaterialInstanceDynamic* DynMat = NpcMesh->CreateDynamicMaterialInstance(0))
        {
            if (NpcData.mood == vno::NPCMood::HAPPY)
                DynMat->SetVectorParameterValue(TEXT("BaseColor"),
                    FLinearColor(0.2f, 0.7f, 0.3f)); // Green = friendly
            else if (NpcData.mood == vno::NPCMood::BUSY)
                DynMat->SetVectorParameterValue(TEXT("BaseColor"),
                    FLinearColor(0.8f, 0.3f, 0.2f)); // Red = busy
            else
                DynMat->SetVectorParameterValue(TEXT("BaseColor"),
                    FLinearColor(0.5f, 0.6f, 0.7f)); // Blue = neutral
        }
    }
}

bool AVNONPCActor::TalkTo(AVNOPlayerCharacter* Player)
{
    if (!Player) return false;

    // Determine dialogue to show
    CurrentDialogue.Reset();

    if (bMoodHappy || !BusyDialogue.Num())
    {
        CurrentDialogue = GreetDialogue;
    }
    else
    {
        CurrentDialogue = BusyDialogue;
    }

    if (CurrentDialogue.Num() == 0)
        CurrentDialogue.Add(TEXT("..."));

    // Show first dialogue line
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
            FString::Printf(TEXT("[%s] %s"), *NpcName, *CurrentDialogue[0]));
    }
    UE_LOG(LogTemp, Log, TEXT("[FR-10] NPC %s: %s"), *NpcName, *CurrentDialogue[0]);

    // Schedule subsequent lines with delays
    UWorld* World = GetWorld();
    if (World)
    {
        float Delay = 3.5f;
        for (int32 i = 1; i < CurrentDialogue.Num(); ++i)
        {
            FString Line = CurrentDialogue[i];
            FString SenderName = NpcName;
            FTimerHandle Handle;
            World->GetTimerManager().SetTimer(Handle,
                [SenderName, Line]()
                {
                    if (GEngine)
                        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
                            FString::Printf(TEXT("[%s] %s"), *SenderName, *Line));
                },
                Delay * i, false);
        }
    }

    // Mark as talked to
    bTalkedTo = true;

    // Notify quest subsystem
    if (World)
    {
        if (UVNOQuestSubsystem* Q = World->GetSubsystem<UVNOQuestSubsystem>())
        {
            Q->OnNPCTalked(NpcId, NpcName);
        }
    }

    return true;
}

void AVNONPCActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);
    if (Cast<AVNOPlayerCharacter>(OtherActor))
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
                FString::Printf(TEXT("[NPC] %s (%s) — nhan E de noi chuyen"), *NpcName, *NpcJob));
    }
}
