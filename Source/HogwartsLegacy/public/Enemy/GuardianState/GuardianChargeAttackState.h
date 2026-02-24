// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/GuardianState/GuardianStateBase.h"
#include "GuardianChargeAttackState.generated.h"

/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UGuardianChargeAttackState : public UGuardianStateBase
{
	GENERATED_BODY()
	
public:
	virtual FName GetStateName() const override { return TEXT("ChargeAttack"); }
	
	virtual void Reset() override;
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void TickState(float DeltaTime) override;
	
private:
	UPROPERTY()
	TObjectPtr<AActor> Target;
	
	float CurTime{0};
	
	float ChargingTime{1.5f};
	float ChargeStrength{5000.f};
	float AttackRange{200.f};
	
	bool bCanCharging{false};
};
