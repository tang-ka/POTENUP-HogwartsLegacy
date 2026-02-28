// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "NiagaraSystem.h"
#include <Interface/Damageable.h>
#include "BasicAttack.generated.h"

UCLASS()
class HOGWARTSLEGACY_API ABasicAttack : public AActor, public IDamageable
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
	
	UPROPERTY()
	AActor* Target;
	UPROPERTY()
	bool ComboAttack;
	UPROPERTY()
	TObjectPtr <AActor> Causer;
	UFUNCTION()
	void SetTarget(AActor* InCauser, AActor* InTarget, bool combo);
	UFUNCTION()
	void NoTarget();

	UPROPERTY(VisibleAnywhere, Category=Collision)
	USphereComponent* CollisionComp;
	UPROPERTY(VisibleAnywhere, Category=Niagara)
	UNiagaraComponent* NiagaraComp;
	UPROPERTY(VisibleAnywhere, Category=Niagara)
	UNiagaraSystem* ExplosionComp;
	UPROPERTY(VisibleAnywhere, Category=Physics)
	float PhysicsForce = 20;
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
		);
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
	UPROPERTY(EditAnywhere)
	TSubclassOf <class UDamageIndicatorWidget> DamageWidgetClass;

	UFUNCTION()
	float GetBasicDamage() const { return BaseDamage; }
	UFUNCTION()
	float GetThirdDamage() const { return ThirdDamage; }
	
	UFUNCTION()
	void ShowDamageWidget(FVector WorldLocation, float DamageAmount);
	
private:
	
	UPROPERTY()
	float BaseDamage = 3;
	UPROPERTY()
	float ThirdDamage = 23;
	
	 // FVector MidPoint;
	 // FVector CurvedPoint;
	 // const float MinCurve = 0.2;
	 // const float MaxCurve = 0.4;
	 // const float MinRadius = 150;
	 // const float MaxRadius = 300;
	 // const float MinAngle = 0;
	 // const float MaxAngle = 180;
	 // bool bAfterBeginPlay = false;
	 //
	 // UFUNCTION()
	 // void MidPointCalculation(
	 // 	FVector InTargetLocation,
	 // 	float InMinCurve,
	 // 	float InMaxCurve,
	 // 	FVector& MidPoint
	 // 	);
	 //
	 // UFUNCTION()
	 // void CurveControlPoint(
	 // 	FVector InMidPoint,
	 // 	FVector InTargetLocation,
	 // 	float InMinRadius,
	 // 	float InMaxRadius,
	 // 	float InMinAngle,
	 // 	float InMaxAngle, 
	 // 	FVector& CurvedPoint
	 // 	);
	 //
	 // UFUNCTION()
	 // void Move(
	 // 	FVector InTargetLocation,
	 // 	FVector InCurvedPoint,
	 // 	float InSpeed
	 // 	);
};
