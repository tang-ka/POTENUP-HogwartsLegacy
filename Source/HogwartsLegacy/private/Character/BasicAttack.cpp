// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasicAttack.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ABasicAttack::ABasicAttack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> NE(TEXT("NiagaraSystem'/Game/KHJ/FX/NS_ArrowTrail_Magic_2.NS_ArrowTrail_Magic_2'"));
	if (NE.Succeeded())
	{
		NiagaraSys = NE.Object;
	}
	
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraSystem"));
	RootComponent = NiagaraComp;
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetCollisionProfileName(TEXT("OverlapAll"));
	CollisionComp->SetSphereRadius(8);
	
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
	
	FVector p0 = GetActorLocation();
	FVector dir = GetActorForwardVector();
	FVector vt = dir * Speed * DeltaTime;
	SetActorLocation(p0 + vt);

}

