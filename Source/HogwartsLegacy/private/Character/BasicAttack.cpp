// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasicAttack.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ABasicAttack::ABasicAttack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NE(TEXT("NiagaraSystem'/Game/KHJ/FX/NS_ArrowTrail_Magic_2.NS_ArrowTrail_Magic_2'"));
	if (NE.Succeeded())
	{
		NiagaraSys = NE.Object;
	}
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetSphereRadius(12);
	RootComponent = CollisionComp;

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraSystem"));

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->SetUpdatedComponent(CollisionComp);
	MovementComp->InitialSpeed = 5000;
	MovementComp->MaxSpeed = 5000;
	MovementComp->bShouldBounce = true;
	MovementComp->Bounciness = 0.3f;
	
}

// Called when the game starts or when spawned
void ABasicAttack::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasicAttack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

