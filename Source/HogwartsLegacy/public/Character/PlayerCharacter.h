// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <GameFramework/SpringArmComponent.h>
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include <Camera/CameraComponent.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include <InputActionValue.h>
#include <Interface/Damageable.h>
#include "PlayerCharacter.generated.h"

class AControllerHUD;
class USpringArmComponent;
class UCameraComponent;
class USceneComponent;
class UPhysicsHandleComponent;

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
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraArmComp;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* PlayerCameraComp;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* WandTipComp;
	UPROPERTY(VisibleAnywhere)
	UPhysicsHandleComponent* PhysicsHandleComp;
	UPROPERTY()
	AControllerHUD* HUD;
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
	bool bCanAttack = false;
	UPROPERTY()
	int ComboCount = 0;
	UPROPERTY()
	bool bComboAttack;
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
	
	UPROPERTY(EditDefaultsOnly, Category=Spell)
	float LeviosoGrabDistance = 500.f;
	UPROPERTY(EditDefaultsOnly, Category=Spell)
	float LeviosoHoldDistance = 300.f;
	
	float LeviosoFloatTimer = 0.f;
	UPROPERTY(EditDefaultsOnly, Category=Spell)
	float LeviosoFloatAmplitude = 10.f;
	UPROPERTY(EditDefaultsOnly, Category=Spell)
	float LeviosoFloatSpeed = 2.f;
	bool bCanUseLevioso = false;
	bool bIsDoingLevioso = false;
	
	FCollisionQueryParams AimCaptureParams;
	UPROPERTY()
	TArray<AActor*> ActorToIgnore;
	UPROPERTY()
	AActor* AimedTarget;
	UPROPERTY()
	TObjectPtr <AActor> LastCachedObj;
#pragma endregion
	
#pragma region Functions
	void Turn(const FInputActionValue& InputValue);
	void LookUp(const FInputActionValue& InputValue);
	void Move(const FInputActionValue& InputValue);
	void Run(const FInputActionValue& InputValue);
	void InputJump(const FInputActionValue& InputValue);
	
	void SkillMouseL(const FInputActionValue& InputValue);
	void SkillSlot1(const FInputActionValue& InputValue);
	void SkillSlot2(const FInputActionValue& InputValue);
	void SkillSlot3(const FInputActionValue& InputValue);
	void SkillSlotQ(const FInputActionValue& InputValue);
	void SkillSlotQLongClick(const FInputActionValue& InputValue);
	void SkillSlotR(const FInputActionValue& InputValue);
	
	UFUNCTION()
	void SpellBasicAttack();
	UFUNCTION()
	void SpellProtego(AProtego* prt);
	UFUNCTION()
	void SpellStupefy(AStupefy* stp);
	UFUNCTION()
	void SpellLumos(USceneComponent* WandTip);
	UFUNCTION()
	void SpellLevioso(UPrimitiveComponent* TargetComp, FVector Location, FRotator Rotation);
	UFUNCTION()
	void SpellDescendo();
	
	UFUNCTION()
	void PlayerMove();
	UFUNCTION()
	void CheckCanAttack(bool& Check);
	UFUNCTION()
	void AimCapture();
	UFUNCTION()
	int GetCurrentHP() const { return this->HP; }
	UFUNCTION()
	int GetDefaultHP() const { return this->MaxHP; }
	UFUNCTION()
	void SetCurrentHP(int NewHP) { this->HP = NewHP; }
#pragma endregion
	
#pragma region IDamageable
	virtual bool ReceiveDamage_Implementation(FDamageInfo DamageInfo) override;
	UFUNCTION()
	virtual int GetHP_Implementation() const override;
	UFUNCTION()
	virtual float GetHPPercent_Implementation() const override;
	UFUNCTION()
	virtual bool IsDead_Implementation() const override;
	//FOnHPChanged HPChangedDelegate;
	//virtual FOnHPChanged& GetOnHPChangedDelegate() override { return HPChangedDelegate; };
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
