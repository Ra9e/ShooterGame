// Shoot Them Up Game. All Rights Reserved.


#include "Components/STUHealthComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Camera/CameraShake.h"
#include "STUUtils.h"
#include "STUGameModeBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

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
    OnHealthChanged.Broadcast(Health, 0.0f);

	AActor* ComponentOwner = GetOwner();
    if (ComponentOwner)
    {
        ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &USTUHealthComponent::OnTakeAmyDamage);
    //    ComponentOwner->OnTakePointDamage.AddDynamic(this, &USTUHealthComponent::OnTakePointDamage);
    //    ComponentOwner->OnTakeRadialDamage.AddDynamic(this, &USTUHealthComponent::OnTakeRadialDamage);
	}
}

// void OnTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
//     class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType,
//     AActor* DamageCauser)
// {
//     const auto FinalDamage = Damage * GetPointDamageModifier(DamagedActor, BoneName);
//     UE_LOG(LogHealthComponent, Dixplay, TEXT("On point damage: %f, final damage: %f, bone: %s"), Damage, *BoneName.ToString());
//     ApplyDamage(FinalDamage, InstigatedBy);
// }
// 
// 
// void OnTakeRadialDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo,
//     class AController* InstigatedBy, AActor* DamageCauser)
// {
//     UE_LOG(LogHealthComponent, Dixplay, TEXT("On radial damage: %f"), Damage);
//     ApplyDamage(Damage, InstigatedBy);
// }

void USTUHealthComponent::OnTakeAmyDamage(
    AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    UE_LOG(LogHealthComponent, Dixplay, TEXT("On radial damage: %f"), Damage);

        // ѕроверка на союзного игрока, если союзник урон не наносим
    // TODO: сделать урон от самого себ€
    // const auto Player = Cast<APawn>(GetOwner());
    // const auto VictimController = Player ? Player->Controller : nullptr;

    // const auto AreEnemies = STUUtils::AreEnemies(VictimController, InstigatedBy);
    // if (!AreEnemies) return;

    if (Damage <= 0.0f || IsDead() || !GetWorld()) return;
    // clamp (значение, если меньше возвращает 0.0f, если больше то макс«доровье) две границы
    SetHealth(Health - Damage);

    GetWorld()->GetTimerManager().ClearTimer(HealTimer);

    if (IsDead())
    {
        Killed(InstigatedBy);
        OnDeath.Broadcast();
    }
    else if (AutoHeal && GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(HealTimer, this, &USTUHealthComponent::HealUpdate, HealUpdateTime, true, HealDelay);
    }

    PlayCameraShake();
}

// void USTUHealthComponent::ApplyDamage(float Damage, AController* InstigatedBy) 
// {
// 
// }

void USTUHealthComponent::HealUpdate()
{
    SetHealth(Health + HealModifier);

    if (IsHealthFull() && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(HealTimer);
    }
}

void USTUHealthComponent::SetHealth(float NewHealth)
{
    const auto NextHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
    const auto HealthDelta = NextHealth - Health;

    Health = NextHealth;
    OnHealthChanged.Broadcast(Health, HealthDelta);
}

bool USTUHealthComponent::TryToAddHealth(float HealthAmount)
{
    if (IsHealthFull() || IsDead()) return false;

    SetHealth(HealthAmount + Health);
    return true;
}

bool USTUHealthComponent::IsHealthFull() const
{
    return FMath::IsNearlyEqual(Health, MaxHealth);
}

void USTUHealthComponent::PlayCameraShake() 
{
    if (IsDead()) return;

    const auto Player = Cast<APawn>(GetOwner());
    if (!Player) return;
    
    const auto Controller = Player->GetController<APlayerController>();
    if (!Controller || !Controller->PlayerCameraManager) return;

    Controller->PlayerCameraManager->StartCameraShake(CameraShake);
}

void USTUHealthComponent::Killed(AController* KillerController) 
{
    if (!GetWorld()) return;

    const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
    if (!GameMode) return;
    
    const auto Player = Cast<APawn>(GetOwner());
    const auto VictimController = Player ? Player->Controller : nullptr;

    GameMode->Killed(KillerController, VictimController);
}
/*
float USTUHealthComponent::GetPointDamageModifier(AActor* DamageActor, const FName& BoneName)
{
    const auto Character = Cast<ACharacter>(DamageActor);
    if (!Character || //
        !Character->GetMesh() || //
        !Character->GetMesh()->GetBodyInstance(BoneName) || //
        !Character->GetMesh()->GetBodyInstance(BoneName)->GetSimplePhysicalMaterial())
        return 1.0f;

    const auto PhysMaterial = Character->GetMesh()->GetBodyInstance(BoneName)->GetSimplePhysicalMaterial();
    if (!PhysMaterial || !DamageModifiers.Contains(PhysMaterial)) return 1.0f;

    return DamageModifiers[PhysMaterial];
}*/