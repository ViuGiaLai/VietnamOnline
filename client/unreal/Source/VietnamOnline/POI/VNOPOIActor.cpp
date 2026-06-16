#include "POI/VNOPOIActor.h"
#include "Player/VNOPlayerCharacter.h"
#include "Quest/VNOQuestSubsystem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"

AVNOPOIActor::AVNOPOIActor()
{
    PrimaryActorTick.bCanEverTick = false;

    CheckInRadius = CreateDefaultSubobject<USphereComponent>(TEXT("CheckInRadius"));
    RootComponent = CheckInRadius;
    CheckInRadius->SetSphereRadius(150.f);
    CheckInRadius->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetRelativeScale3D(FVector(2.f, 2.f, 4.f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeFinder.Succeeded()) Mesh->SetStaticMesh(CubeFinder.Object);
}

void AVNOPOIActor::InitFromData(const vno::POI& PoiData)
{
    PoiId = UTF8_TO_TCHAR(PoiData.poi_id.c_str());
    NameVi = UTF8_TO_TCHAR(PoiData.name_vi.c_str());
    EntryFeeVnd = PoiData.entry_fee_vnd;
    ExpReward = PoiData.exp_reward;
    CheckInRadius->SetSphereRadius(static_cast<float>(PoiData.check_in_radius_m) * 100.f);
}

bool AVNOPOIActor::TryCheckIn(AVNOPlayerCharacter* Player)
{
    if (!Player || bCheckedIn) return false;
    float Dist = FVector::Dist(Player->GetActorLocation(), GetActorLocation());
    if (Dist > CheckInRadius->GetScaledSphereRadius()) return false;

    if (EntryFeeVnd > 0 && Player->GetVndBalance() < EntryFeeVnd)
    {
        UE_LOG(LogTemp, Warning, TEXT("[POI] Khong du VND cho %s"), *NameVi);
        return false;
    }

    bCheckedIn = true;
    Player->AddVnd(EntryFeeVnd > 0 ? -EntryFeeVnd : 200000);
    if (UWorld* World = GetWorld())
    {
        if (UVNOQuestSubsystem* Q = World->GetSubsystem<UVNOQuestSubsystem>())
        {
            Q->OnPoiCheckedIn(PoiId);
        }
    }
    UE_LOG(LogTemp, Log, TEXT("[POI] Check-in: %s (FR-07) +%d EXP"), *NameVi, ExpReward);
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
        FString::Printf(TEXT("Check-in: %s"), *NameVi));
    return true;
}
