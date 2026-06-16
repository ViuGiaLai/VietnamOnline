#include "Player/VNOAnimInstance.h"
#include "Player/VNOPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UVNOAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    const AVNOPlayerCharacter* Char = Cast<AVNOPlayerCharacter>(TryGetPawnOwner());
    if (!Char) return;
    Speed = Char->GetVelocity().Size();
    bIsSprinting = Char->IsSprinting();
    if (const UCharacterMovementComponent* Move = Char->GetCharacterMovement())
    {
        bIsInAir = Move->IsFalling();
    }
}
