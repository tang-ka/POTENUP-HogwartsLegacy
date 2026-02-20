// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasicAttack.h"
#include "Character/PlayerCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABasicAttack::ABasicAttack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> NE(TEXT("NiagaraSystem'/Game/KHJ/FX/NS_ArrowTrail_Magic_2.NS_ArrowTrail_Magic_2'"));
	if (NE.Succeeded())
	{
		NiagaraSys = NE.Object;
	}
	
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraSystem"));
	RootComponent = NiagaraComp;
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionComp->SetSphereRadius(8);
	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABasicAttack::OnComponentBeginOverlap);
}

// Called when the game starts or when spawned
void ABasicAttack::BeginPlay()
{
	Super::BeginPlay();
	
	MidPointCalculation(TargetLocation, MinCurve, MaxCurve, MidPoint);
	CurveControlPoint(MidPoint, TargetLocation, MinRadius, MaxRadius, MinAngle, MaxAngle, CurvedPoint);
	AfterBeginPlay = true;
}

// Called every frame
void ABasicAttack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (AfterBeginPlay)
	{
		Move(TargetLocation, CurvedPoint, Speed);
		if (GetActorLocation() == TargetLocation)
		{
			SetTimer += DeltaTime;
			if (SetTimer > 1)
			{
				Destroy();
			}
		}
	}
}

void ABasicAttack::SetTarget(AActor* atk)
{
	TargetLocation = atk->GetActorLocation();
}

void ABasicAttack::NoTarget()
{
	TargetLocation = GetActorLocation() + GetActorForwardVector() * Distance;
}

void ABasicAttack::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()))
	{
		GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, TEXT("OnHit"));
		Destroy();
	}
}


void ABasicAttack::MidPointCalculation(FVector InTargetLocation, float InMinCurve, float InMaxCurve, FVector& InMidPoint)
{
	FVector	p = InTargetLocation - GetActorLocation();
	float Dist = UKismetMathLibrary::RandomFloatInRange(InMinCurve, InMaxCurve);
	InMidPoint = Dist * p + GetActorLocation();
}

void ABasicAttack::CurveControlPoint(FVector InMidPoint, FVector InTargetLocation, float InMinRadius, float InMaxRadius, float InMinAngle, float InMaxAngle, FVector& InCurvedPoint)
{
	float p = UKismetMathLibrary::RandomFloatInRange(InMinRadius, InMaxRadius);
	float k = UKismetMathLibrary::RandomFloatInRange(InMinAngle, InMaxAngle);
	const FRotator Rot = UKismetMathLibrary::FindLookAtRotation(InMidPoint, InTargetLocation);
	FVector x, y, z;
	UKismetMathLibrary::BreakRotIntoAxes(Rot, x, y, z);
	CurvedPoint = InMidPoint + UKismetMathLibrary::RotateAngleAxis(y*p, k, x);
}

void ABasicAttack::Move(FVector InTargetLocation, FVector InCurvedPoint, float InSpeed)
{
	float Time = GetWorld()->GetTimeSeconds();
	InCurvedPoint = UKismetMathLibrary::VInterpTo_Constant(InCurvedPoint, InTargetLocation, Time, InSpeed);
	SetActorLocation(UKismetMathLibrary::VInterpTo_Constant(GetActorLocation(), InCurvedPoint, Time, InSpeed));
}

