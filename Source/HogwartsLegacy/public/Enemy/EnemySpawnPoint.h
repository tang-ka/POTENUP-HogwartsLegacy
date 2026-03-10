// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnPoint.generated.h"

class USphereComponent;
class UArrowComponent;

UCLASS()
class HOGWARTSLEGACY_API AEnemySpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool IsSafeToSpawn() const;
	FTransform GetSafeSpawnPoint() const;
	
	// UFUNCTION()
	// void OnSphereBeginOverlap(
	// 	UPrimitiveComponent* OverlappedComponent,
	// 	AActor* OtherActor,
	// 	UPrimitiveComponent* OtherComp,
	// 	int32 OtherBodyIndex,
	// 	bool bFromSweep,
	// 	const FHitResult& SweepResult
	// );
	//
	// UFUNCTION()
	// void OnSphereEndOverlap(
	// 	UPrimitiveComponent* OverlappedComponent,
	// 	AActor* OtherActor,
	// 	UPrimitiveComponent* OtherComp,
	// 	int32 OtherBodyIndex
	// );
	
public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> RootComp;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UArrowComponent> ArrowComp;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SpawnCheckSphere;
	
	bool IsSafe{true};
	
	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	float SphereTraceRadius{500.f};
};
