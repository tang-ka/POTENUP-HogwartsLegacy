// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	
	UPROPERTY(VisibleAnywhere, Category=Movement)
	class UProjectileMovementComponent* ProjectileMovementComp;
	UPROPERTY(VisibleAnywhere, Category=Collision)
	class USphereComponent* CollisionComp;
	UPROPERTY(VisibleAnywhere, Category=Niagara)
	class UParticleSystemComponent* NiagaraComp;

};
