// Shoot Them Up Game. All Rights Reserved.


#include "AI/Services/STU_FireService.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "STUUtils.h"
#include "Components/STUWeaponComponent.h"

USTU_FireService::USTU_FireService() 
{
    NodeName = "Fire service";
}

void USTU_FireService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    const auto Controller = OwnerComp.GetAIOwner();
    const auto Blackboard = OwnerComp.GetBlackboardComponent();

    const auto HasAim = Blackboard && Blackboard->GetValueAsObject(EnemyActorKey.SelectedKeyName);
    if (Controller)
    {
        const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(Controller->GetPawn());
        if (WeaponComponent)
        {
            HasAim ? WeaponComponent->StartFire() : WeaponComponent->StopFire();
        }
    }

    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}