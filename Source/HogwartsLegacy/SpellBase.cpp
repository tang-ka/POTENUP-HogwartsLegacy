#include "SpellBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ControllerHUD.h"

ASpellBase::ASpellBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = CollisionComponent;
	CollisionComponent->InitSphereRadius(15.0f);
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	CollisionComponent->OnComponentHit.AddDynamic(this, &ASpellBase::OnHit);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = SpellSpeed;
	ProjectileMovement->MaxSpeed = SpellSpeed;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	InitialLifeSpan = 3.0f;
}

void ASpellBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASpellBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpellBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, FVector NormalImpulse,
						const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherActor != SpellOwner)
	{
		UGameplayStatics::ApplyDamage(
			OtherActor,
			BaseDamage,
			nullptr,
			this,
			UDamageType::StaticClass()
		);

		AControllerHUD* HUD = Cast<AControllerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
		if (HUD)
		{
			HUD->DrawDamageWidget(OtherActor->GetActorLocation(), BaseDamage);
		}

		Destroy();
	}
}