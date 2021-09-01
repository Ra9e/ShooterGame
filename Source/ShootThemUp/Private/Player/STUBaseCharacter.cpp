// Shoot Them Up Game. All Rights Reserved.


#include "Player/STUBaseCharacter.h"
#include "Components/STUCharacterMovementComponent.h"
#include "Components/STUHealthComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/STUWeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseCharacter, All, All);

// Sets default values
ASTUBaseCharacter::ASTUBaseCharacter(const FObjectInitializer& ObjInit) 
    : Super(ObjInit.SetDefaultSubobjectClass<USTUCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    HealthComponent = CreateDefaultSubobject<USTUHealthComponent>("HealthComponent");

    WeaponComponent = CreateDefaultSubobject<USTUWeaponComponent>("WeaponComponent");
}

// Called when the game starts or when spawned
void ASTUBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    check(HealthComponent);
    check(GetCharacterMovement());
    check(GetMesh());

    OnHealthChanged(HealthComponent->GetHealth(), 0.0f);
    HealthComponent->OnDeath.AddUObject(this, &ASTUBaseCharacter::OnDeath);
    HealthComponent->OnHealthChanged.AddUObject(this, &ASTUBaseCharacter::OnHealthChanged);

    LandedDelegate.AddDynamic(this, &ASTUBaseCharacter::OnGroundLanded);

}

void ASTUBaseCharacter::OnHealthChanged(float Health, float HealthDelta)
{

}

// Called every frame
void ASTUBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //TakeDamage(0.1f, FDamageEvent{}, Controller, this); 
}

bool ASTUBaseCharacter::IsRunning() const
{
    return false;
}

float ASTUBaseCharacter::GetMovementDirection() const
{
    if (GetVelocity().IsZero()) return 0.0f;

    const auto VelocityNormal = GetVelocity().GetSafeNormal(); // Нормаль вектора скорости
    const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), VelocityNormal)); // скалярное произведение вызывается DotProduct
    const auto CrossProduct = FVector::CrossProduct(GetActorForwardVector(), VelocityNormal); // Векторное произведение вызывается CrossProduct
    const auto Degrees = FMath::RadiansToDegrees(AngleBetween); // переводим угол в градусы из радианов

    // переводим угол в градусы из радианов и умножаем на знак (+-) (Sign) координаты CrossProduct.Z
    return CrossProduct.IsZero() ? Degrees : Degrees * FMath::Sign(CrossProduct.Z);
}

void ASTUBaseCharacter::OnDeath() 
{
    UE_LOG(LogBaseCharacter, Display, TEXT("Player %s is dead"), *GetName());

    // PlayAnimMontage(DeathAnimMontage);
    GetCharacterMovement()->DisableMovement();
    SetLifeSpan(LifeSpanOnDeath);

    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    WeaponComponent->StartFire();
    WeaponComponent->Zoom(false);

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);

    UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
}

void ASTUBaseCharacter::OnGroundLanded(const FHitResult& Hit) 
{
    const auto FallVelocityZ = -GetVelocity().Z;

    UE_LOG(LogBaseCharacter, Display, TEXT("On landed: %f"), FallVelocityZ);

    if (FallVelocityZ < LandedDamageVelocity.X) return;
    
    const auto FinalDamage = FMath::GetMappedRangeValueClamped(LandedDamageVelocity, LandedDamage, FallVelocityZ);
    UE_LOG(LogBaseCharacter, Display, TEXT("Final Damage: %f"), FinalDamage);
    TakeDamage(FinalDamage, FDamageEvent{}, nullptr, nullptr);
}

void ASTUBaseCharacter::SetPlayerColor(const FLinearColor& Color) 
{
    const auto MaterialInst = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
    if (!MaterialInst) return;

    MaterialInst->SetVectorParameterValue(MaterialColorName, Color);
}
