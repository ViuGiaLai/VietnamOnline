#include "Food/VNOFoodShop.h"
#include "Player/VNOPlayerCharacter.h"
#include "Player/VNOPlayerState.h"
#include "Quest/VNOQuestSubsystem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "TimerManager.h"

AVNOFoodShop::AVNOFoodShop()
{
    PrimaryActorTick.bCanEverTick = false;

    InteractRadius = CreateDefaultSubobject<USphereComponent>(TEXT("InteractRadius"));
    RootComponent = InteractRadius;
    InteractRadius->SetSphereRadius(250.f);
    InteractRadius->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    InteractRadius->SetGenerateOverlapEvents(true);

    StallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StallMesh"));
    StallMesh->SetupAttachment(RootComponent);
    StallMesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
    StallMesh->SetRelativeScale3D(FVector(2.f, 2.f, 1.5f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylFinder(
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (CylFinder.Succeeded())
        StallMesh->SetStaticMesh(CylFinder.Object);

    Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
    Label->SetupAttachment(StallMesh);
    Label->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
    Label->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
    Label->SetHorizontalAlignment(EHTA_Center);
    Label->SetTextRenderColor(FColor::White);
    Label->SetWorldSize(35.f);
}

void AVNOFoodShop::InitFromData(const vno::FoodItem& Item, const FVector& Location)
{
    FoodItemName = UTF8_TO_TCHAR(Item.name.c_str());
    FoodPrice = Item.price;
    ShopName = FString::Printf(TEXT("Quan %s"), *FoodItemName);
    SetActorLocation(Location);

    if (Label)
        Label->SetText(FText::FromString(ShopName));

    // Build menu
    FVNOFoodOption Opt;
    Opt.Name = FoodItemName;
    Opt.Price = FoodPrice;
    Opt.Region = UTF8_TO_TCHAR(Item.region.c_str());
    Opt.bAvailable = true;

    // Buff description based on type
    switch (Item.buff.buff_type)
    {
    case vno::BuffType::STAMINA_REGEN:
        Opt.BuffDescription = FString::Printf(TEXT("Hoi Stamina +%.0f%% trong %d phut"),
            Item.buff.value_primary * 100.f, Item.buff.duration_seconds / 60);
        break;
    case vno::BuffType::SPEED_BOOST:
        Opt.BuffDescription = FString::Printf(TEXT("Tang toc +%.0f%% trong %d phut"),
            Item.buff.value_primary * 100.f, Item.buff.duration_seconds / 60);
        break;
    case vno::BuffType::EXP_BONUS:
        Opt.BuffDescription = FString::Printf(TEXT("EXP +%.0f%% trong %d phut"),
            Item.buff.value_primary * 100.f, Item.buff.duration_seconds / 60);
        break;
    case vno::BuffType::PRICE_DISCOUNT:
        Opt.BuffDescription = FString::Printf(TEXT("Giam gia -%.0f%% trong %d phut"),
            Item.buff.value_primary * 100.f, Item.buff.duration_seconds / 60);
        break;
    case vno::BuffType::PHOTO_BONUS:
        Opt.BuffDescription = TEXT("Tang diem chup anh 50%");
        break;
    default:
        Opt.BuffDescription = TEXT("Dac san dia phuong!");
        break;
    }
    Menu.Add(Opt);
    BuffData.Add(Item.buff);
}

bool AVNOFoodShop::TryOrderFood(AVNOPlayerCharacter* Player, int32 FoodIndex)
{
    if (!Player || !Menu.IsValidIndex(FoodIndex)) return false;
    const FVNOFoodOption& Selected = Menu[FoodIndex];
    if (!Selected.bAvailable) return false;

    if (Player->GetVndBalance() < Selected.Price)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Food] Khong du VND cho %s: can %lld"), *Selected.Name, Selected.Price);
        return false;
    }

    Player->AddVnd(-Selected.Price);

    // Notify quest subsystem
    if (UWorld* W = GetWorld())
    {
        if (UVNOQuestSubsystem* Q = W->GetSubsystem<UVNOQuestSubsystem>())
            Q->OnFoodPurchased(Selected.Name);
    }

    // Apply buff - store on player
    if (BuffData.IsValidIndex(FoodIndex))
    {
        const vno::BuffEntry& Buff = BuffData[FoodIndex];
        FString BuffTypeStr;
        switch (Buff.buff_type)
        {
        case vno::BuffType::STAMINA_REGEN: BuffTypeStr = TEXT("STAMINA_REGEN"); break;
        case vno::BuffType::SPEED_BOOST:   BuffTypeStr = TEXT("SPEED_BOOST"); break;
        case vno::BuffType::EXP_BONUS:     BuffTypeStr = TEXT("EXP_BONUS"); break;
        case vno::BuffType::PRICE_DISCOUNT:BuffTypeStr = TEXT("PRICE_DISCOUNT"); break;
        case vno::BuffType::PHOTO_BONUS:   BuffTypeStr = TEXT("PHOTO_BONUS"); break;
        case vno::BuffType::SOCIAL_BONUS:  BuffTypeStr = TEXT("SOCIAL_BONUS"); break;
        case vno::BuffType::ANTI_NAUSEA:   BuffTypeStr = TEXT("ANTI_NAUSEA"); break;
        default:                           BuffTypeStr = TEXT("PHOTO_BONUS"); break;
        }
        Player->ApplyBuff(BuffTypeStr, Selected.Name, Buff.value_primary, Buff.duration_seconds);
    }

    UE_LOG(LogTemp, Log, TEXT("[FR-12] Da mua %s — %lld VND — %s"), *Selected.Name, Selected.Price, *Selected.BuffDescription);
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Orange,
            FString::Printf(TEXT("🍜 Da goi: %s! %s"), *Selected.Name, *Selected.BuffDescription));

    return true;
}

void AVNOFoodShop::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);
    if (Cast<AVNOPlayerCharacter>(OtherActor))
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
                FString::Printf(TEXT("🍜 %s — nhan E de goi mon"), *ShopName));
    }
}
