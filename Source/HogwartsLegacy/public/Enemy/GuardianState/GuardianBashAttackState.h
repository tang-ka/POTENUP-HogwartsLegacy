// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/GuardianState/GuardianStateBase.h"
#include "GuardianBashAttackState.generated.h"

/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UGuardianBashAttackState : public UGuardianStateBase
{
	GENERATED_BODY()

public:
	virtual FName GetStateName() const override { return TEXT("BashAttack"); }
	
	virtual void Reset() override;
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void TickState(float DeltaTime) override;
	
	void BashAttack();
	
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> Target;
	
	float ChargingTime{1.5f};
	float BashStrength{5000.f};
	
	float CurTime{0.f};
	bool bIsBashing{false};
};
