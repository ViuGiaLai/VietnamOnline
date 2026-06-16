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

void UVNOQuestSubsystem::LogActiveQuests() const
{
    for (const FVNOQuestRuntime& R : RuntimeQuests)
    {
        if (R.bActive)
            UE_LOG(LogTemp, Log, TEXT("  [ACTIVE] %s [%d/%d]"), *R.Title, R.StepsDone, R.StepsTotal);
    }
}
