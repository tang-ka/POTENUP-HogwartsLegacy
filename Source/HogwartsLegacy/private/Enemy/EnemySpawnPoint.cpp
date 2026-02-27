// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemySpawnPoint.h"

#include "Components/ArrowComponent.h"

// Sets default values
AEnemySpawnPoint::AEnemySpawnPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	ArrowComp->SetupAttachment(RootComp);
	ArrowComp->ArrowColor = FColor::Red;
	ArrowComp->ArrowSize = 0.5f;
	ArrowComp->SetHiddenInGame(false);
}

// Called when the game starts or when spawned
void AEnemySpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugSphere
	(
		GetWorld(),
		GetActorLocation(),
		10.f,
		12,
		FColor::Blue,
		true,
		-1.f,
		0,
		0.5f);
}

// Called every frame
void AEnemySpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AEnemySpawnPoint::IsSafeToSpawn()
{
	return true;
}
