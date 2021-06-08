// Shoot Them Up Game. All Rights Reserved.


#include "Components/STUHealthComponent.h"
#include "GameFramework//Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All);

// Sets default values for this component's properties
USTUHealthComponent::USTUHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void USTUHealthComponent::BeginPlay()
{
	Super::BeginPlay();

    check(MaxHealth > 0);

    SetHealth(MaxHealth);
    OnHealthChanged.Broadcast(Health);

	AActor* ComponentOwner = GetOwner();
    if (ComponentOwner)
    {
        ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &USTUHealthComponent::OnTakeAmyDamage);
	}
}

void USTUHealthComponent::OnTakeAmyDamage(
    AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f || IsDead() || !GetWorld()) return;
	// clamp (значение, если меньше возвращает 0.0f, если больше то макс«доровье) две границы
    SetHealth(Health - Damage);

    GetWorld()->GetTimerManager().ClearTimer(HealTimer);

	if (IsDead())
	{
        OnDeath.Broadcast();
	}
    else if (AutoHeal && GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(HealTimer, this, &USTUHealthComponent::HealUpdate, HealUpdateTime, true, HealDelay);
    }
}

void USTUHealthComponent::HealUpdate()
{
    SetHealth(Health + HealModifier);

    if (FMath::IsNearlyEqual(Health, MaxHealth) && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(HealTimer);
    }
}

void USTUHealthComponent::SetHealth(float NewHealth)
{
    Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(Health);
}
