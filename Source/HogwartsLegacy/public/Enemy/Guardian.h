// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GuardianState/GuardianChargeAttackState.h"
#include "Interface/Damageable.h"
#include "Guardian.generated.h"

class USphereComponent;
class UEnemyFSMComponent;

UCLASS()
class HOGWARTSLEGACY_API AGuardian : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	AGuardian();

public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
#pragma region Charge Attack
	void ExecuteChargeToTarget(AActor* Target, const float ChargeStrength);
	void SnapToAttackLocation(const AActor* Target, const float AttackRange);
	void ExecuteMeleeAttack();

	UFUNCTION()
	void OnChargeAttackHitBoxOverlap(UPrimitiveComponent* OverlappedComponent,
	                                 AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                                 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
#pragma endregion


#pragma region Movement
	UFUNCTION(BlueprintCallable)
	void SetCharacterMaxWalkSpeed(float InMaxWalkSpeed) const;
	UFUNCTION(BlueprintCallable)
	void MoveToActor(AActor* Target, float AcceptanceRadius = -1.0f) const;
	UFUNCTION(BlueprintCallable)
	void StopMovementImmediately() const;
#pragma endregion

#pragma region HitBox
	UFUNCTION(BlueprintCallable)
	void SetActiveChargeAttackHitBox(bool bIsActivate) const;
	UFUNCTION(BlueprintCallable)
	void SetActiveBashAttackHitBox(bool bIsActivate) const;
#pragma endregion

public:
#pragma region IDamageable
	virtual bool ReceiveDamage_Implementation(FDamageInfo DamageInfo) override;
	virtual int GetHP_Implementation() const override;
	virtual bool IsDead_Implementation() const override;
#pragma endregion

public:
#pragma region Test
	void SetMaterialColor(FLinearColor Color);
	void ResetMaterialColor();
#pragma endregion

	int GetCurrentHP() const { return HP; }
	UEnemyFSMComponent* GetFSMComponent() const { return FSMComponent; }
	AActor* GetTargetActor() const { return TargetActor; }

private:
	void InitHP();

private:
	UPROPERTY(VisibleAnywhere, Category="Stats")
	int32 HP{};

	UPROPERTY(VisibleAnywhere, Category="Stats")
	int32 MaxHP{150};

private:
	UPROPERTY(VisibleAnywhere, Category="FSMComponent")
	TObjectPtr<UEnemyFSMComponent> FSMComponent;

	// UPROPERTY(EditAnywhere, Category="FSMComponent")
	// TObjectPtr<UEnemyFSMComponent> ForCopyFSMComponent;

	UPROPERTY(EditAnywhere, Category="HitBox")
	TObjectPtr<USphereComponent> ChargeAttackHitBox;

	UPROPERTY(EditAnywhere, Category="HitBox")
	TObjectPtr<USphereComponent> BashAttackHitBox;;

	UPROPERTY(EditAnywhere, Category="Target")
	TObjectPtr<AActor> TargetActor;

	FLinearColor DefaultColor;
};
