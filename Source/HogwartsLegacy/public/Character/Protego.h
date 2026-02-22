// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Protego.generated.h"

UCLASS()
class HOGWARTSLEGACY_API AProtego : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProtego();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, Category=Mesh)
	UStaticMeshComponent* SphereMesh;

	float Duration = 0;

};
