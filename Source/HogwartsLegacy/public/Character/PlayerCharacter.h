// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class HOGWARTSLEGACY_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	UPROPERTY(VisibleAnywhere, Category=Camera)
	class USpringArmComponent* CameraArmComp;
	UPROPERTY(VisibleAnywhere, Category=Camera)
	class UCameraComponent* PlayerCameraComp;
	
public:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputMappingContext* IMC_Default;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* IA_LookUp;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* IA_Turn;
	
	// Horizontal Rotation Input
	void Turn(const struct FInputActionValue& inputValue);
	// Vertical Rotation Input
	void LookUp(const struct FInputActionValue& inputValue);
};
