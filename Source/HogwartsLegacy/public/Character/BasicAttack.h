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
	UPROPERTY()
	AControllerHUD* HUD;
	
	UPROPERTY(VisibleAnywhere, Category=Collision)
	USphereComponent* CollisionComp;
	UPROPERTY(VisibleAnywhere, Category=Niagara)
	UNiagaraComponent* NiagaraComp;
	UPROPERTY(VisibleAnywhere, Category=Niagara)
	UNiagaraSystem* ExplosionComp;
	UPROPERTY(VisibleAnywhere, Category=Physics)
	float PhysicsForce = 20;
	UPROPERTY()
	USoundBase* SpellSound;
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
	float Speed = 2500;
	UPROPERTY()
	float Distance = 2000;
	
	UPROPERTY()
	int Damage = 3;

};
