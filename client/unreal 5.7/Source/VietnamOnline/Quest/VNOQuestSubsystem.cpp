#include "Quest/VNOQuestSubsystem.h"
#include "vno/data/HoiAnZoneData.hpp"

void UVNOQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    const auto Registry = vno::data::createQuestRegistry();
    for (const auto& Q : Registry)
    {
        FVNOQuestRuntime R;
        R.QuestId = UTF8_TO_TCHAR(Q.quest_id.c_str());
        R.Title = UTF8_TO_TCHAR(Q.title.c_str());
        R.StepsTotal = static_cast<int32>(Q.steps.size());
        RuntimeQuests.Add(R);
    }
    AcceptQuest(TEXT("QUEST-MAIN-01"));
}

bool UVNOQuestSubsystem::AcceptQuest(const FString& QuestId)
{
    int32 Active = 0;
    for (const FVNOQuestRuntime& R : RuntimeQuests)
        if (R.bActive) Active++;
    if (Active >= 3) return false;

    for (FVNOQuestRuntime& R : RuntimeQuests)
    {
        if (R.QuestId == QuestId && !R.bActive)
        {
            R.bActive = true;
            UE_LOG(LogTemp, Log, TEXT("[Quest] Nhan: %s (FR-08)"), *R.Title);
            return true;
        }
    }
    return false;
}

void UVNOQuestSubsystem::OnPoiCheckedIn(const FString& PoiId)
{
    for (FVNOQuestRuntime& R : RuntimeQuests)
    {
        if (!R.bActive) continue;
        if (R.QuestId == TEXT("QUEST-MAIN-01") && PoiId.Contains(TEXT("POI-001")))
        {
            R.StepsDone = FMath::Min(R.StepsTotal, R.StepsDone + 1);
            if (R.StepsDone >= R.StepsTotal)
            {
                R.bActive = false;
                UE_LOG(LogTemp, Log, TEXT("[Quest] Hoan thanh: %s"), *R.Title);
            }
        }
    }
}

void UVNOQuestSubsystem::OnVehicleRented(const FString& VehicleId)
{
    for (FVNOQuestRuntime& R : RuntimeQuests)
    {
        if (!R.bActive) continue;
        if (R.QuestId == TEXT("QUEST-SIDE-03") && VehicleId.Contains(TEXT("HONDA")))
        {
            R.StepsDone = FMath::Min(R.StepsTotal, R.StepsDone + 1);
            if (R.StepsDone >= R.StepsTotal)
            {
                R.bActive = false;
                UE_LOG(LogTemp, Log, TEXT("[Quest] Hoan thanh: %s (thue xe) "), *R.Title);
            }
        }
    }
}

void UVNOQuestSubsystem::OnNPCTalked(const FString& NpcId, const FString& NpcName)
{
    // Try to unlock quests gated by this NPC
    const auto Registry = vno::data::createQuestRegistry();
    for (const auto& Q : Registry)
    {
        if (FString(Q.giver_npc_id.c_str()) == NpcId)
        {
            // Accept quest if it's available
            AcceptQuest(UTF8_TO_TCHAR(Q.quest_id.c_str()));
            UE_LOG(LogTemp, Log, TEXT("[Quest] NPC %s mo khoa: %s"), *NpcName,
                UTF8_TO_TCHAR(Q.title.c_str()));
            if (GEngine)
                GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Green,
                    FString::Printf(TEXT("[Quest] Nhan quest tu %s: %s"),
                        *NpcName, UTF8_TO_TCHAR(Q.title.c_str())));
        }
    }

    // Advance TALK_NPC quest steps
    for (FVNOQuestRuntime& R : RuntimeQuests)
    {
        if (!R.bActive) continue;

        // QUEST-SIDE-01: Talk to 3 people
        if (R.QuestId == TEXT("QUEST-SIDE-01"))
        {
            R.StepsDone = FMath::Min(R.StepsTotal, R.StepsDone + 1);
            UE_LOG(LogTemp, Log, TEXT("[Quest] %s: da noi chuyen %d/3 nguoi"),
                *R.Title, R.StepsDone);
            if (R.StepsDone >= R.StepsTotal)
            {
                R.bActive = false;
                UE_LOG(LogTemp, Log, TEXT("[Quest] Hoan thanh: %s"), *R.Title);
            }
            return;
        }

        // QUEST-SIDE-05: Talk to 5 NPCs
        if (R.QuestId == TEXT("QUEST-SIDE-05"))
        {
            R.StepsDone = FMath::Min(R.StepsTotal, R.StepsDone + 1);
            UE_LOG(LogTemp, Log, TEXT("[Quest] %s: da gap %d/5 nguoi"),
                *R.Title, R.StepsDone);
            if (R.StepsDone >= R.StepsTotal)
            {
                R.bActive = false;
                UE_LOG(LogTemp, Log, TEXT("[Quest] Hoan thanh: %s"), *R.Title);
            }
            return;
        }
    }
}

void UVNOQuestSubsystem::OnFoodPurchased(const FString& FoodName)
{
    for (FVNOQuestRuntime& R : RuntimeQuests)
    {
        if (!R.bActive) continue;

        // QUEST-SIDE-02: Buy Cao Lau exactly
        if (R.QuestId == TEXT("QUEST-SIDE-02") && FoodName.Contains(TEXT("Cao Lau")))
        {
            R.StepsDone = FMath::Min(R.StepsTotal, R.StepsDone + 1);
            if (R.StepsDone >= R.StepsTotal)
            {
                R.bActive = false;
                UE_LOG(LogTemp, Log, TEXT("[Quest] Hoan thanh: %s (da mua Cao Lau)"), *R.Title);
            }
            return;
        }

        // QUEST-DAILY-01: Buy any 3 food/drinks
        if (R.QuestId == TEXT("QUEST-DAILY-01"))
        {
            R.StepsDone = FMath::Min(R.StepsTotal, R.StepsDone + 1);
            if (R.StepsDone >= R.StepsTotal)
            {
                R.bActive = false;
                UE_LOG(LogTemp, Log, TEXT("[Quest] Hoan thanh: %s (da mua 3 do)"), *R.Title);
            }
            return;
        }
    }
}

void UVNOQuestSubsystem::RestoreQuestProgress(const FString& QuestId, bool bActive, int32 StepsDone)
{
    for (FVNOQuestRuntime& R : RuntimeQuests)
    {
        if (R.QuestId == QuestId)
        {
            R.bActive = bActive;
            R.StepsDone = StepsDone;
            UE_LOG(LogTemp, Log, TEXT("[Quest] Restore: %s %s [%d/%d]"),
                *QuestId, bActive ? TEXT("ACTIVE") : TEXT("inactive"), StepsDone, R.StepsTotal);
            return;
        }
    }
}

void UVNOQuestSubsystem::LogActiveQuests() const
{
    for (const FVNOQuestRuntime& R : RuntimeQuests)
    {
        if (R.bActive)
            UE_LOG(LogTemp, Log, TEXT("  [ACTIVE] %s [%d/%d]"), *R.Title, R.StepsDone, R.StepsTotal);
    }
}
