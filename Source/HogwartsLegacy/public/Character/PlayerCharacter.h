// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Camera/CameraComponent.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include <InputActionValue.h>
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
	USpringArmComponent* CameraArmComp;
	UPROPERTY(VisibleAnywhere, Category=Camera)
	UCameraComponent* PlayerCameraComp;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* IMC_Player;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_LookUp;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Turn;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Move;
	UPROPERTY(EditAnywhere, Category=PlayerSetting)
	float WalkSpeed = 300;
	FVector WalkDirection;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Run;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Jump;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Attack;
	bool bCanAttack = true;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Skill;
	
	UPROPERTY(EditDefaultsOnly,Category=Spell)
	TSubclassOf <class ABasicAttack> BasicAttacks;
	
	// Horizontal Rotation Input
	void Turn(const FInputActionValue& InputValue);
	// Vertical Rotation Input
	void LookUp(const FInputActionValue& InputValue);
	
	void Move(const FInputActionValue& InputValue);
	// Switch between Walk Mode and Run Mode
	void Run(const FInputActionValue& InputValue);
	
	void InputJump(const FInputActionValue& InputValue);

	void SpellBasicAttack(const FInputActionValue& InputValue);
	
	void SpellSkill(const FInputActionValue& InputValue);
	
	void PlayerMove();
	
};
