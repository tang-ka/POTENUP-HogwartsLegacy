// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GuardianAnim.generated.h"

enum class EEnemyState : uint8;
class AGuardian;
/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UGuardianAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	void HandleStateChanged(EEnemyState NewState);
	void PlayBasicDamagedMontage();
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	FVector2D MoveDirection; // Y: Forward/Backward, X: Right/Left
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	EEnemyState GuardianState;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<AGuardian> OwningGuardian;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> BasicDamagedMontage;
	
};
