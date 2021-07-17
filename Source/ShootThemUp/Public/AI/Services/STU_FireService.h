// Shoot Them Up Game. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "STU_FireService.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTTHEMUP_API USTU_FireService : public UBTService
{
	GENERATED_BODY()
	
public:
    USTU_FireService();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FBlackboardKeySelector EnemyActorKey;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
