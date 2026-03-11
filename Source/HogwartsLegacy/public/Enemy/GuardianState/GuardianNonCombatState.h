// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/GuardianState/GuardianStateBase.h"
#include "GuardianNonCombatState.generated.h"

/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UGuardianSleepState : public UGuardianStateBase
{
	GENERATED_BODY()
	
public:
	virtual FName GetStateName() const override { return TEXT("Sleep"); }

	virtual void TickState(float DeltaTime) override;
};

UCLASS()
class HOGWARTSLEGACY_API UGuardianIdleState : public UGuardianStateBase
{
	GENERATED_BODY()
	
public:
	virtual FName GetStateName() const override { return TEXT("Idle"); }
	
	virtual void OnEnter() override;
	virtual void TickState(float DeltaTime) override;
	
protected:
	virtual void Reset() override;
	
private:
	void LookAtTarget(float DeltaTime) const;
	void DecideTarget() const;

private:
	float IdleTime{0.f};
	float IdleDuration{2.f};
};
