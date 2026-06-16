#include "POI/VNOPOIActor.h"
#include "Player/VNOPlayerCharacter.h"
#include "Quest/VNOQuestSubsystem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace {
    void ApplyColor(UStaticMeshComponent* Comp, const FLinearColor& C)
    {
        if (!Comp) return;
        if (UMaterialInstanceDynamic* DynMat = Comp->CreateDynamicMaterialInstance(0))
            DynMat->SetVectorParameterValue(TEXT("BaseColor"), C);
    }
}

AVNOPOIActor::AVNOPOIActor()
{
    PrimaryActorTick.bCanEverTick = false;

    CheckInRadius = CreateDefaultSubobject<USphereComponent>(TEXT("CheckInRadius"));
    RootComponent = CheckInRadius;
    CheckInRadius->SetSphereRadius(150.f);
    CheckInRadius->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    // Cylinder base
    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    BaseMesh->SetupAttachment(RootComponent);
    BaseMesh->SetRelativeLocation(FVector(0.f, 0.f, 40.f));
    BaseMesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.8f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (CylinderFinder.Succeeded())
        BaseMesh->SetStaticMesh(CylinderFinder.Object);

    // Cone/Pyramid marker on top
    MarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarkerMesh"));
    MarkerMesh->SetupAttachment(BaseMesh);
    MarkerMesh->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
    MarkerMesh->SetRelativeScale3D(FVector(0.4f, 0.4f, 1.0f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeFinder(
        TEXT("/Engine/BasicShapes/Cone.Cone"));
    if (ConeFinder.Succeeded())
        MarkerMesh->SetStaticMesh(ConeFinder.Object);

    // Floating name label
    NameLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NameLabel"));
    NameLabel->SetupAttachment(BaseMesh);
    NameLabel->SetRelativeLocation(FVector(0.f, 0.f, 140.f));
    NameLabel->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
    NameLabel->SetHorizontalAlignment(EHTA_Center);
    NameLabel->SetTextRenderColor(FColor::White);
    NameLabel->SetWorldSize(40.f);
    NameLabel->SetText(FText::FromString(TEXT("")));
}

void AVNOPOIActor::InitFromData(const vno::POI& PoiData)
{
    PoiId = UTF8_TO_TCHAR(PoiData.poi_id.c_str());
    NameVi = UTF8_TO_TCHAR(PoiData.name_vi.c_str());
    EntryFeeVnd = PoiData.entry_fee_vnd;
    ExpReward = PoiData.exp_reward;
    CheckInRadius->SetSphereRadius(static_cast<float>(PoiData.check_in_radius_m) * 100.f);

    // Set category string and color
    switch (PoiData.category)
    {
    case vno::POICategory::HISTORY:
        CategoryStr = TEXT("Lich su");      PoiColor = FLinearColor(0.8f, 0.4f, 0.2f); break; // Brown
    case vno::POICategory::NATURE:
        CategoryStr = TEXT("Thien nhien");  PoiColor = FLinearColor(0.2f, 0.7f, 0.3f); break; // Green
    case vno::POICategory::SPIRITUAL:
        CategoryStr = TEXT("Tam linh");     PoiColor = FLinearColor(0.6f, 0.2f, 0.6f); break; // Purple
    case vno::POICategory::FOOD:
        CategoryStr = TEXT("Am thuc");      PoiColor = FLinearColor(0.9f, 0.5f, 0.1f); break; // Orange
    case vno::POICategory::ENTERTAINMENT:
        CategoryStr = TEXT("Giai tri");     PoiColor = FLinearColor(0.1f, 0.6f, 0.8f); break; // Blue
    case vno::POICategory::HERITAGE:
        CategoryStr = TEXT("Di san");       PoiColor = FLinearColor(0.8f, 0.1f, 0.1f); break; // Red
    default:
        CategoryStr = TEXT("Khac");         PoiColor = FLinearColor(0.5f, 0.5f, 0.5f); break; // Gray
    }

    SetupVisualsByCategory(PoiColor);

    // Show name label
    if (NameLabel)
        NameLabel->SetText(FText::FromString(NameVi));

    UE_LOG(LogTemp, Verbose, TEXT("[POI] Init: %s [%s]"), *NameVi, *CategoryStr);
}

void AVNOPOIActor::SetupVisualsByCategory(const FLinearColor& Color)
{
    ApplyColor(BaseMesh, Color);
    ApplyColor(MarkerMesh, FLinearColor(Color.R * 1.2f, Color.G * 1.2f, Color.B * 1.2f));
}

void AVNOPOIActor::SetCheckedInVisuals()
{
    bCheckedIn = true;
    float Avg = (PoiColor.R + PoiColor.G + PoiColor.B) / 3.f;
    FLinearColor Greyed(Avg * 0.6f, Avg * 0.6f, Avg * 0.6f);
    ApplyColor(BaseMesh, Greyed);
    ApplyColor(MarkerMesh, Greyed);
    if (NameLabel)
        NameLabel->SetTextRenderColor(FColor(160, 160, 160, 180));
}

bool AVNOPOIActor::TryCheckIn(AVNOPlayerCharacter* Player)
{
    if (!Player || bCheckedIn) return false;
    float Dist = FVector::Dist(Player->GetActorLocation(), GetActorLocation());
    if (Dist > CheckInRadius->GetScaledSphereRadius()) return false;

    const int64 Fee = EntryFeeVnd;
    if (Fee > 0 && Player->GetVndBalance() < Fee)
    {
        UE_LOG(LogTemp, Warning, TEXT("[POI] Khong du VND cho %s (can %lld)"), *NameVi, Fee);
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
                FString::Printf(TEXT("Khong du VND! Can %lld"), Fee));
        return false;
    }

    Player->AddVnd(Fee > 0 ? -Fee : 200000);
    SetCheckedInVisuals();

    if (UWorld* World = GetWorld())
    {
        if (UVNOQuestSubsystem* Q = World->GetSubsystem<UVNOQuestSubsystem>())
            Q->OnPoiCheckedIn(PoiId);
    }

    UE_LOG(LogTemp, Log, TEXT("[POI] Check-in: %s [%s] +%d EXP +%lld VND"),
        *NameVi, *CategoryStr, ExpReward, Fee > 0 ? -Fee : 200000);

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Green,
            FString::Printf(TEXT("Check-in: %s (+%d EXP)"), *NameVi, ExpReward));

    return true;
}

FLinearColor AVNOPOIActor::GetMapColor() const
{
    return bCheckedIn
        ? FLinearColor(PoiColor.R * 0.5f, PoiColor.G * 0.5f, PoiColor.B * 0.5f, 0.5f)
        : PoiColor;
}
