// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GuardianShatterComponent.generated.h"

class UProceduralMeshComponent;
class USkeletalMesh;
class USkeletalMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOGWARTSLEGACY_API UGuardianShatterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGuardianShatterComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void InitializeShatter();
	UFUNCTION(BlueprintCallable)
	void TriggerShatter();
	
private:
	bool EnsureSourceMesh();
	UProceduralMeshComponent* GetOrCreateProceduralMesh();
	void CopySkeletalMeshToProceduralMesh(UProceduralMeshComponent* InProceduralMeshComponent);

private:
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> MeshComp{};
	
	UPROPERTY()
	TObjectPtr<USkeletalMesh> Mesh{};

	UPROPERTY(Transient)
	TObjectPtr<UProceduralMeshComponent> ProceduralMeshComp{};
	
	bool bIsShattered{false};
};
