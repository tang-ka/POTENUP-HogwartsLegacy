// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include <InputActionValue.h>
#include <Interface/Damageable.h>
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USceneComponent;

UCLASS()
class HOGWARTSLEGACY_API APlayerCharacter : public ACharacter, public IDamageable
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
	
#pragma region Components
	UPROPERTY(VisibleAnywhere, Category=Camera)
	USpringArmComponent* CameraArmComp;
	UPROPERTY(VisibleAnywhere, Category=Camera)
	UCameraComponent* PlayerCameraComp;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* WandTipComp;
#pragma endregion
	
#pragma region Inputs
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* IMC_Player;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_LookUp;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Turn;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Move;
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
#pragma endregion
	
#pragma region Properties
	UPROPERTY()
	FVector WalkDirection;
	UPROPERTY()
	bool CanAttack;
	UPROPERTY()
	int ComboCount = 0;
	UPROPERTY()
	bool ComboAttack;
	UPROPERTY()
	FTimerHandle ComboTimerHandle;
	
	UPROPERTY(EditDefaultsOnly, Category=Spell)
	TSubclassOf <class ALumos> Lumoses;
	UPROPERTY()
	ALumos* lumos;
	
	UPROPERTY(EditDefaultsOnly, Category=Spell)
	TSubclassOf <class ABasicAttack> BasicAttacks;
	UPROPERTY()
	ABasicAttack* attack;
	
	UPROPERTY(EditDefaultsOnly, Category=Spell)
	TSubclassOf <class AProtego> Protegos;
	UPROPERTY()
	AProtego* protego;
	
	UPROPERTY(EditDefaultsOnly, Category=Spell)
	TSubclassOf <class AStupefy> Stupefies;
	UPROPERTY()
	AStupefy* stupefy;

	UPROPERTY()
	TArray<AActor*> ActorToIgnore;
	UPROPERTY()
	TObjectPtr <AActor> AimedTarget;
	UPROPERTY()
	TObjectPtr <AActor> LastCachedObj;
	UPROPERTY()
	TObjectPtr <UMaterialInterface> OutlineMat;
	UPROPERTY()
	TObjectPtr <UMaterialInterface> ShineMat;
#pragma endregion
	
#pragma region Functions
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
	void SpellStupefy(const FInputActionValue& InputValue);
	void SpellSkillSlotR(const FInputActionValue& InputValue);
	
	void SpellLumos();
	
	UFUNCTION()
	void PlayerMove();
	UFUNCTION()
	void CheckCanAttack(bool& Check);
	UFUNCTION()
	void AimCapture();
	UFUNCTION()
	int GetCurrentHP() const { return this->HP; }
	UFUNCTION()
	void SetCurrentHP(int NewHP) { this->HP = NewHP; }
	UFUNCTION()
	int GetMaxHP() const { return this->MaxHP; }
#pragma endregion
	
#pragma region IDamageable
	virtual bool ReceiveDamage_Implementation(FDamageInfo DamageInfo) override;
	UFUNCTION()
	virtual int GetHP_Implementation() const override;
	UFUNCTION()
	virtual bool IsDead_Implementation() const override;
#pragma endregion

#pragma region Privates
private:
	UPROPERTY()
	int HP = 100;
	UPROPERTY()
	int MaxHP = 100;
	UPROPERTY()
	float WalkSpeed = 300;
	UPROPERTY()
	int ViewDistance = 2000;
	UPROPERTY()
	float ViewSphereRadius = 125;
#pragma endregion

};
