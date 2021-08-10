// Shoot Them Up Game. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "STUEmenyEnvQueryContext.generated.h"


UCLASS()
class SHOOTTHEMUP_API USTUEmenyEnvQueryContext : public UEnvQueryContext
{
	GENERATED_BODY()
	
public:
    virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FName EnemyActroKeyName = "EnemyActor";
};
