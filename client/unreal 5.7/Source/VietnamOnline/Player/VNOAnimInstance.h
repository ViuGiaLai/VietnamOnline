#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "VNOAnimInstance.generated.h"

class AVNOPlayerCharacter;

/** Locomotion blend (walk / sprint) — doc A.3 animation blend */
UCLASS()
class VIETNAMONLINE_API UVNOAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    UPROPERTY(BlueprintReadOnly, Category = "VNO|Anim")
    float Speed = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|Anim")
    bool bIsSprinting = false;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|Anim")
    bool bIsInAir = false;
};
