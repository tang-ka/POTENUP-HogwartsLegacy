// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_SkillSlot1;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_SkillSlot2;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_SkillSlot3;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_SkillSlotQ;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_SkillSlotR;
	
	UPROPERTY(EditDefaultsOnly,Category=Spell)
	TSubclassOf <class ABasicAttack> BasicAttacks;
	UPROPERTY()
	ABasicAttack* atk;
	UPROPERTY(EditDefaultsOnly,Category=Spell)
	TSubclassOf <class AProtego> Protegos;
	UPROPERTY()
	AProtego* prt;
	UPROPERTY()
	int BasicAttackComboCount = 0;
	
	UPROPERTY()
	TArray<AActor*> ActorToIgnore;
	UPROPERTY()
	AActor* AimTarget;
	UPROPERTY()
	int ViewDistance = 2000;
	UPROPERTY()
	float ViewSphereRadius = 125;
	UPROPERTY()
	AActor* LastCachedObj;
	UPROPERTY()
	TObjectPtr <UMaterialInterface> OutlineMat;
	UPROPERTY()
	TObjectPtr <UMaterialInterface> ShineMat;
	
	void Turn(const FInputActionValue& InputValue);
	void LookUp(const FInputActionValue& InputValue);
	void Move(const FInputActionValue& InputValue);
	void Run(const FInputActionValue& InputValue);
	void InputJump(const FInputActionValue& InputValue);

	void SpellBasicAttack(const FInputActionValue& InputValue);
	void SpellSkillSlot1(const FInputActionValue& InputValue);
	void SpellSkillSlot2(const FInputActionValue& InputValue);
	void SpellSkillSlot3(const FInputActionValue& InputValue);
	void SpellSkillSlotQ(const FInputActionValue& InputValue);
	void SpellSkillSlotR(const FInputActionValue& InputValue);
	
	UFUNCTION()
	void PlayerMove();

	UFUNCTION()
	void AimCapture(AActor* Target);
	
	
};
