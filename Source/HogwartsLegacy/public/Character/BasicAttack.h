// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "NiagaraSystem.h"
#include "UI/ControllerHUD.h"
#include "BasicAttack.generated.h"

class AControllerHUD;

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
	
	UPROPERTY(EditAnywhere, Category=Collision)
	USphereComponent* CollisionComp;
	UPROPERTY(EditAnywhere, Category=Niagara)
	UNiagaraComponent* NiagaraComp;
	UPROPERTY(EditAnywhere, Category=Niagara)
	UNiagaraSystem* ExplosionComp;
	
	UPROPERTY()
	AActor* Target;
	UPROPERTY()
	bool ComboAttack;
	UPROPERTY()
	TObjectPtr <AActor> Causer;
	UPROPERTY(VisibleAnywhere, Category=Physics)
	float PhysicsForce = 20;
	UPROPERTY(EditAnywhere, Category=CameraShake)
	TSubclassOf<UCameraShakeBase> HitCameraShakeClass;
	UPROPERTY(EditAnywhere)
	USoundBase* SpellSound;
	UPROPERTY()
	AControllerHUD* HUD;
	
	UPROPERTY()
	FVector TargetLocation;
	UPROPERTY()
	float Speed = 2500;
	UPROPERTY()
	float Distance = 2000;
	UPROPERTY()
	FVector StartLocation;
	UPROPERTY()
	float ElapsedTime = 0.0f;
	UPROPERTY()
	float FlightTime = 0.25f;
	UPROPERTY()
	int Damage = 3;
	
	UFUNCTION()
	void SetTarget(AActor* InCauser, AActor* InTarget, bool combo);
	UFUNCTION()
	void NoTarget();
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
};
