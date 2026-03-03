

#include "Character/BasicAttack.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interface/DamageableHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "UI/EnemyHPWidget.h"


class IDamageable;
ABasicAttack::ABasicAttack()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetSphereRadius(8);
	CollisionComp->SetCollisionProfileName(TEXT("Spell"));
	CollisionComp->SetGenerateOverlapEvents(true);
	
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABasicAttack::OnOverlap); // Enemy
	CollisionComp->OnComponentHit.AddDynamic(this, &ABasicAttack::OnHit); // WorldObject
	RootComponent = CollisionComp;
	
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraSystem"));
	NiagaraComp->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> TempComp(TEXT("NiagaraSystem'/Game/KHJ/FX/NS_BasicAttack.NS_BasicAttack'"));
	if (TempComp.Succeeded())
	{
		NiagaraComp->SetAsset(TempComp.Object);
	}
	
	ExplosionComp = CreateDefaultSubobject<UNiagaraSystem>(TEXT("ExplosionNiagaraSystem"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ExpTempComp(TEXT("ExplosionNiagaraSystem'/Game/KHJ/FX/NS_BasicAttack_Explosion.NS_BasicAttack_Explosion'"));
	if (ExpTempComp.Succeeded())
	{
		ExplosionComp = ExpTempComp.Object;
	}
	
	Tags.Add(FName("Spell"));
	InitialLifeSpan = 2.5f;
	
}

void ABasicAttack::BeginPlay()
{
	Super::BeginPlay();
	
	if (SpellSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SpellSound);
	}
	
	HUD = Cast<AControllerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
}

void ABasicAttack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SetActorLocation(UKismetMathLibrary::VInterpTo_Constant(GetActorLocation(), TargetLocation, DeltaTime, Speed), true);
}

void ABasicAttack::SetTarget(AActor* causer, AActor* target, bool combo)
{
	Target = target;
	TargetLocation = target->GetActorLocation() + UKismetMathLibrary::MakeVector(0,0,40);
	ComboAttack = combo;
}

void ABasicAttack::NoTarget()
{
	TargetLocation = this->GetActorLocation() + this->GetActorForwardVector() * Distance;
}

// When Begin Overlap On The Enemy
void ABasicAttack::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!DamageableHelpers::IsDead(Target))
	{
		int Damage = ComboAttack ? GetThirdDamage() : GetBasicDamage();
		DamageableHelpers::ApplyDamage(Target, Causer, Damage);
		if (!IsValid(HUD))
		{
			return;
		}
		HUD->DrawDamageWidget(GetActorLocation() + FVector(0, 0, 50.f), Damage);
		HUD->UpdateEnemyHPWidget(Target);
		// ShowDamageWidget(GetActorLocation() + FVector(0, 0, 50.f), Damage);
		// GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Damage: %d"), Damage));
	}
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionComp, SweepResult.TraceStart);
	Destroy();
}

// When Hit On The WorldObject
void ABasicAttack::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherComp->IsSimulatingPhysics())
	{
		CollisionComp->SetSimulatePhysics(true);
		OtherComp->AddImpulseAtLocation(GetVelocity() * PhysicsForce, GetActorLocation());
	}
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionComp, Hit.TraceStart);
	Destroy();
}



//
// void ABasicAttack::MidPointCalculation(FVector InTargetLocation, float InMinCurve, float InMaxCurve, FVector& InMidPoint)
// {
// 	FVector	p = InTargetLocation - GetActorLocation();
// 	float Dist = UKismetMathLibrary::RandomFloatInRange(InMinCurve, InMaxCurve);
// 	InMidPoint = Dist * p + GetActorLocation();
// }
//
// void ABasicAttack::CurveControlPoint(FVector InMidPoint, FVector InTargetLocation,
// 	float InMinRadius, float InMaxRadius, float InMinAngle, float InMaxAngle, FVector& InCurvedPoint)
// {
// 	float p = UKismetMathLibrary::RandomFloatInRange(InMinRadius, InMaxRadius);
// 	float k = UKismetMathLibrary::RandomFloatInRange(InMinAngle, InMaxAngle);
// 	const FRotator Rot = UKismetMathLibrary::FindLookAtRotation(InMidPoint, InTargetLocation);
// 	FVector x, y, z;
// 	UKismetMathLibrary::BreakRotIntoAxes(Rot, x, y, z);
// 	CurvedPoint = InMidPoint + UKismetMathLibrary::RotateAngleAxis(y*p, k, x);
// }
//
// void ABasicAttack::Move(FVector InTargetLocation, FVector InCurvedPoint, float InSpeed)
// {
// 	float Time = GetWorld()->GetTimeSeconds();
// 	InCurvedPoint = UKismetMathLibrary::VInterpTo_Constant(InCurvedPoint, InTargetLocation, Time, InSpeed);
// 	SetActorLocation(UKismetMathLibrary::VInterpTo_Constant(GetActorLocation(), InCurvedPoint, Time, InSpeed));
// }
