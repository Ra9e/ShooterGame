// Shoot Them Up Game. All Rights Reserved.


#include "AI/STUAICharacter.h"
#include "AI/STUAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/STUAIWeaponComponent.h"

ASTUAICharacter::ASTUAICharacter(const FObjectInitializer& ObjInit)
    : Super(ObjInit.SetDefaultSubobjectClass<USTUAIWeaponComponent>("WeaponComponent"))
{
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = ASTUAIController::StaticClass();

    bUseControllerRotationYaw = false;
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->bUseControllerDesiredRotation = true;
        GetCharacterMovement()->RotationRate = FRotator(0.0f, 250.0f, 0.0f);
    }
}