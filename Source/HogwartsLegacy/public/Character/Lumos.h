// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "Lumos.generated.h"

class UStaticMeshComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USceneComponent;

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
	virtual void BeginDestroy() override;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* SpellSphereComp;
	UPROPERTY(EditAnywhere, Category=Niagara)
	UNiagaraSystem* StartNiagara;
	UPROPERTY(EditAnywhere, Category=Niagara)
	UNiagaraComponent* SpawnedNiagara;
	UPROPERTY()
	USkeletalMeshComponent* Caster;
	UPROPERTY()
	USkeletalMeshSocket const* CasterSocket;
	
	UFUNCTION()
	void SetLocation(USkeletalMeshComponent* Mesh, USkeletalMeshSocket const* WandTip);
};
