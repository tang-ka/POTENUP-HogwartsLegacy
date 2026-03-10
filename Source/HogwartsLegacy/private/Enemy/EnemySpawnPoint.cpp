// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemySpawnPoint.h"

#include "HogwartsLegacy.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"

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
	
	SpawnCheckSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SpawnCheckSphere"));
	SpawnCheckSphere->SetupAttachment(RootComp);
	SpawnCheckSphere->SetSphereRadius(200.f);
	SpawnCheckSphere->SetCollisionProfileName(TEXT("EnemySpawnPoint"));
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
		FColor::Yellow,
		true,
		-1.f,
		0,
		0.5f);
	
	// SpawnCheckSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawnPoint::OnSphereBeginOverlap);
	// SpawnCheckSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemySpawnPoint::OnSphereEndOverlap);
}

// Called every frame
void AEnemySpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AEnemySpawnPoint::IsSafeToSpawn() const
{
	// GetSafeSpawnPoint().GetLocation 위치에서 GetActorLocation()으로 SphereTrace를 수행하여 장애물이 있는지 확인
	FHitResult HitResult;
	FVector Start = GetSafeSpawnPoint().GetLocation();
	FVector End = GetActorLocation();
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 자신은 무시
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(SphereTraceRadius),
		QueryParams
	);
	
	// DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Red : FColor::Green, false, 2.f, 0, 2.f);
	// DrawDebugSphere(GetWorld(), End, SphereTraceRadius, 12, bHit ? FColor::Red : FColor::Green, false, 2.f);
	// if (bHit)
	// {
	// 	DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 20.f, 12, FColor::Cyan, false, 2.f);
	// }
	// 	
	return !bHit; // 장애물이 없으면 안전
}

FTransform AEnemySpawnPoint::GetSafeSpawnPoint() const
{
	FTransform SpawnTransform = GetActorTransform();
	SpawnTransform.SetLocation(SpawnTransform.GetLocation() + FVector::UpVector * 1000.f);
	return SpawnTransform;
}

// void AEnemySpawnPoint::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
// 	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// 	// PRINT_LOG(TEXT("Enemy Spawn Point: Detected overlap with %s"), *OtherActor->GetName());
// 	IsSafe = false;
// }
//
// void AEnemySpawnPoint::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
// 	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
// {
// 	// PRINT_LOG(TEXT("Enemy Spawn Point: Detected end of overlap with %s"), *OtherActor->GetName());
// 	IsSafe = true;
// }
