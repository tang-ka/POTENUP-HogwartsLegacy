// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/GuardianState/GuardianStateBase.h"
#include "GuardianDamagedState.generated.h"

/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UGuardianDamagedState : public UGuardianStateBase
{
	GENERATED_BODY()
	
public:
	virtual FName GetStateName() const override { return TEXT("Damaged"); }
	
	virtual void Reset() override;
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void TickState(float DeltaTime) override;
	
private:
	float DamagedDuration{1.0f};
	float CurTime{0.f};	
};
