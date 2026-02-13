// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
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
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(VisibleAnywhere, Category=Camera)
	class USpringArmComponent* CameraArmComp;
	UPROPERTY(VisibleAnywhere, Category=Camera)
	class UCameraComponent* PlayerCameraComp;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputMappingContext* IMC_Default;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* IA_LookUp;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Turn;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* IA_Move;
	UPROPERTY(EditAnywhere, Category=PlayerSetting)
	float WalkSpeed = 300;
	FVector WalkDirection;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* IA_Run;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* IA_Jump;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* IA_Attack;
	
	UPROPERTY(EditDefaultsOnly,Category=Spell)
	TSubclassOf <class ABasicAttack> BasicAttacks;
	
	// Horizontal Rotation Input
	void Turn(const struct FInputActionValue& InputValue);
	// Vertical Rotation Input
	void LookUp(const struct FInputActionValue& InputValue);
	
	void Move(const struct FInputActionValue& InputValue);
	// Switch between Walk Mode and Run Mode
	void Run(const struct FInputActionValue& InputValue);
	
	void InputJump(const struct FInputActionValue& InputValue);

	void SpellBasicAttack(const struct FInputActionValue& InputValue);
	
	void PlayerMove();
	
};
