// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "BasicAttack.generated.h"

UCLASS()
class HOGWARTSLEGACY_API ABasicAttack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasicAttack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetTarget(AActor* Actor);
	void NoTarget();

	UPROPERTY(VisibleAnywhere, Category=Collision)
	class USphereComponent* CollisionComp;
	UPROPERTY(VisibleAnywhere, Category=Niagara)
	class UNiagaraComponent* NiagaraComp;
	UFUNCTION()
	void OnComponentBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& SweepResult
		);
	
	FVector TargetLocation;
	FVector MidPoint;
	FVector CurvedPoint;
	const float Speed = 2000;
	const float Distance = 2000;
	const float MinCurve = 0.2;
	const float MaxCurve = 0.4;
	const float MinRadius = 150;
	const float MaxRadius = 300;
	const float MinAngle = 0;
	const float MaxAngle = 180;
	bool bAfterBeginPlay = false;
	float Duration = 0;
	
	void MidPointCalculation(
		FVector InTargetLocation,
		float InMinCurve,
		float InMaxCurve,
		FVector& MidPoint
		);
	
	void CurveControlPoint(
		FVector InMidPoint,
		FVector InTargetLocation,
		float InMinRadius,
		float InMaxRadius,
		float InMinAngle,
		float InMaxAngle, 
		FVector& CurvedPoint
		);
	
	void Move(
		FVector InTargetLocation,
		FVector InCurvedPoint,
		float InSpeed
		);
};
