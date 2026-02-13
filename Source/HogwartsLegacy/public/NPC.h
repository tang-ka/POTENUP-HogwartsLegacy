// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC.generated.h"

UCLASS()
class HOGWARTSLEGACY_API ANPC : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MovementSpeed = 450.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Support AI")
	class ACharacter* PlayerRef;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float FollowDistance = 80.0f; // 기본값 200
};
