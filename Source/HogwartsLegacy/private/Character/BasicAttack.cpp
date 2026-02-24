// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasicAttack.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ABasicAttack::ABasicAttack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetSphereRadius(8);
	CollisionComp->SetCollisionProfileName(TEXT("Spell"));
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->SetSimulatePhysics(true);
	
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABasicAttack::OnOverlap); // Enemy
	CollisionComp->OnComponentHit.AddDynamic(this, &ABasicAttack::OnHit); // WorldObject
	RootComponent = CollisionComp;
	
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraSystem"));

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> TempComp(TEXT("NiagaraSystem'/Game/KHJ/FX/NS_BasicAttack.NS_BasicAttack'"));
	if (TempComp.Succeeded())
	{
		NiagaraComp->SetAsset(TempComp.Object);
	}
	
	NiagaraComp->SetupAttachment(RootComponent);
	
	InitialLifeSpan = 2.5f;
	
}

// Called when the game starts or when spawned
void ABasicAttack::BeginPlay()
{
	Super::BeginPlay();
	
	// MidPointCalculation(TargetLocation, MinCurve, MaxCurve, MidPoint);
	// CurveControlPoint(MidPoint, TargetLocation, MinRadius, MaxRadius, MinAngle, MaxAngle, CurvedPoint);
	// bAfterBeginPlay = true;
}

// Called every frame
void ABasicAttack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	SetActorLocation(UKismetMathLibrary::VInterpTo_Constant(GetActorLocation(), TargetLocation, DeltaTime, Speed), true);
		
	if (GetActorLocation() == TargetLocation)
	{
		Duration += DeltaTime;
		if (Duration > 0.75)
		{
			Destroy();
		}
	}
}

void ABasicAttack::SetTarget(AActor* atk)
{
	TargetLocation = atk->GetActorLocation();
}

void ABasicAttack::NoTarget()
{
	TargetLocation = this->GetActorLocation() + this->GetActorForwardVector() * Distance;
}

// When Hit On The WorldObject
void ABasicAttack::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	if (OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * PhysicsForce, GetActorLocation());
		//GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, TEXT("OnHit, AddImpulse"));
	} else
	{
		//GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, TEXT("OnHit"));
	}
	Destroy();
}

// When Begin Overlap On The Enemy
void ABasicAttack::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, TEXT("OnBeginOverlap"));
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
