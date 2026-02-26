// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Lumos.generated.h"

class UStaticMeshComponent;
class UNiagaraSystem;

UCLASS()
class HOGWARTSLEGACY_API ALumos : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALumos();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* SpellSphereComp;
	UPROPERTY(VisibleAnywhere, Category=Niagara)
	UNiagaraSystem* StartNiagara;
	// UPROPERTY()
	// APlayerCharacter* Player;
	UPROPERTY()
	FVector Location;
	
	UFUNCTION()
	void SetLocation(FVector loc);
};
