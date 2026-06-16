#include "Vehicles/VNOVehicleRentalStation.h"
#include "Player/VNOPlayerCharacter.h"
#include "Player/VNOPlayerState.h"
#include "Quest/VNOQuestSubsystem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "TimerManager.h"

AVNOVehicleRentalStation::AVNOVehicleRentalStation()
{
    PrimaryActorTick.bCanEverTick = false;

    InteractionRadius = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionRadius"));
    RootComponent = InteractionRadius;
    InteractionRadius->SetSphereRadius(300.f);
    InteractionRadius->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    InteractionRadius->SetGenerateOverlapEvents(true);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetRelativeScale3D(FVector(3.f, 3.f, 2.f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (CylinderFinder.Succeeded())
        Mesh->SetStaticMesh(CylinderFinder.Object);
}

void AVNOVehicleRentalStation::InitFromData(const vno::RentalStation& Data)
{
    StationId = UTF8_TO_TCHAR(Data.station_id.c_str());
    StationName = UTF8_TO_TCHAR(Data.name.c_str());
    LocX = Data.x;
    LocY = Data.y;
    RefreshAvailableVehicles();
}

void AVNOVehicleRentalStation::RefreshAvailableVehicles()
{
    AvailableVehicles.Empty();
    const auto VehicleRegistry = vno::data::createVehicleRegistry();
    for (const auto& V : VehicleRegistry)
    {
        FVNOVehicleOption Opt;
        switch (V.type)
        {
        case vno::VehicleType::BICYCLE:       Opt.Name = TEXT("Xe Dap"); break;
        case vno::VehicleType::HONDA_WAVE:    Opt.Name = TEXT("Honda Wave"); break;
        case vno::VehicleType::YAMAHA_EXCITER: Opt.Name = TEXT("Yamaha Exciter"); break;
        case vno::VehicleType::HONDA_SH:      Opt.Name = TEXT("Honda SH"); break;
        case vno::VehicleType::XICH_LO:       Opt.Name = TEXT("Xich Lo"); break;
        default: Opt.Name = TEXT("Unknown"); break;
        }
        Opt.CostPerHour = V.rental_cost_hour;
        Opt.MaxSpeedKmh = V.max_speed_kmh;
        Opt.bAlleyAccess = V.alley_access;
        Opt.bAvailable = (V.unlock_phase <= 1);
        AvailableVehicles.Add(Opt);
    }
}

bool AVNOVehicleRentalStation::TryRentVehicle(AVNOPlayerCharacter* Player, int32 VehicleIndex)
{
    if (!Player || !AvailableVehicles.IsValidIndex(VehicleIndex))
        return false;

    const FVNOVehicleOption& Selected = AvailableVehicles[VehicleIndex];
    if (!Selected.bAvailable)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Rental] %s chua mo khoa"), *Selected.Name);
        return false;
    }

    if (Player->GetVndBalance() < Selected.CostPerHour)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Rental] Khong du VND: can %lld, co %lld"), Selected.CostPerHour, Player->GetVndBalance());
        return false;
    }

    Player->AddVnd(-Selected.CostPerHour);

    UE_LOG(LogTemp, Log, TEXT("[FR-09] Thue %s tai %s — %lld VND/gio"), *Selected.Name, *StationName, Selected.CostPerHour);
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
            FString::Printf(TEXT("Da thue %s! %lld VND/gio"), *Selected.Name, Selected.CostPerHour));

    if (UWorld* World = GetWorld())
    {
        if (UVNOQuestSubsystem* Q = World->GetSubsystem<UVNOQuestSubsystem>())
        {
            if (Selected.Name.Contains(TEXT("Honda")))
            {
                Q->OnVehicleRented(TEXT("HONDA_WAVE"));
            }
        }
    }

    return true;
}

void AVNOVehicleRentalStation::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);
    if (AVNOPlayerCharacter* PC = Cast<AVNOPlayerCharacter>(OtherActor))
    {
        UE_LOG(LogTemp, Log, TEXT("[Rental] %s — nhan E de thue xe"), *StationName);
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow,
                FString::Printf(TEXT("Tram: %s — nhan E de thue xe"), *StationName));
    }
}

void AVNOVehicleRentalStation::NotifyActorEndOverlap(AActor* OtherActor)
{
    Super::NotifyActorEndOverlap(OtherActor);
}
