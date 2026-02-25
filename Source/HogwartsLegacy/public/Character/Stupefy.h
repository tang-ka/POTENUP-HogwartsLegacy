// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "NiagaraSystem.h"
#include "Stupefy.generated.h"

UCLASS()
class HOGWARTSLEGACY_API AStupefy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStupefy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetTarget(AActor* Target);
	void NoTarget();

	UPROPERTY(VisibleAnywhere, Category=Collision)
	USphereComponent* CollisionComp;
	UPROPERTY(EditAnywhere, Category=Niagara)
	UNiagaraComponent* NiagaraComp;
	UPROPERTY(EditAnywhere, Category=Niagara)
	UNiagaraSystem* ExplosionComp;
	UFUNCTION()
	void OnOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);
	
	UPROPERTY()
	FVector TargetLocation;
	UPROPERTY()
	float Speed = 2000;
	UPROPERTY()
	float Distance = 2000;
};
