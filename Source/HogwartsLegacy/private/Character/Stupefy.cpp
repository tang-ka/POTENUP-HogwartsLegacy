// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Stupefy.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AStupefy::AStupefy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetSphereRadius(8);
	CollisionComp->SetCollisionProfileName(TEXT("Spell"));
	CollisionComp->SetGenerateOverlapEvents(true);

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AStupefy::OnOverlap);
	RootComponent = CollisionComp;
	
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraSystem"));

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> TempComp(TEXT("NiagaraSystem'/Game/KHJ/FX/NS_Stupefy.NS_Stupefy'"));
	if (TempComp.Succeeded())
	{
		NiagaraComp->SetAsset(TempComp.Object);
	}
	
	ExplosionComp = CreateDefaultSubobject<UNiagaraSystem>(TEXT("ExplosionNiagaraSystem"));
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> ExpTempComp(TEXT("ExplosionNiagaraSystem'/Game/KHJ/FX/NS_BasicAttack_Explosion.NS_BasicAttack_Explosion'"));
	if (ExpTempComp.Succeeded())
	{
		ExplosionComp = ExpTempComp.Object;
	}
	
	NiagaraComp->SetupAttachment(RootComponent);
	
	Tags.Add(FName("Spell"));
	InitialLifeSpan = 2.5f;
}

// Called when the game starts or when spawned
void AStupefy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStupefy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SetActorLocation(UKismetMathLibrary::VInterpTo_Constant(GetActorLocation(), TargetLocation, DeltaTime, Speed), true);
}

void AStupefy::SetTarget(AActor* Target)
{
	TargetLocation = Target->GetActorLocation();
}

void AStupefy::NoTarget()
{
	TargetLocation = this->GetActorLocation() + this->GetActorForwardVector() * Distance;
}

void AStupefy::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionComp, SweepResult.TraceStart);
	Destroy();
}