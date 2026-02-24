// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/GuardianState/GuardianStateBase.h"
#include "GuardianDeadState.generated.h"

/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UGuardianDeadState : public UGuardianStateBase
{
	GENERATED_BODY()
	
public:
	virtual FName GetStateName() const override { return TEXT("Dead"); }
	
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void TickState(float DeltaTime) override;
};
