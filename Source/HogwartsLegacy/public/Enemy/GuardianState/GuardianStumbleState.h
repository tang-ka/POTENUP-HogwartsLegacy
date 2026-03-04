// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/GuardianState/GuardianStateBase.h"
#include "GuardianStumbleState.generated.h"

/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UGuardianStumbleState : public UGuardianStateBase
{
	GENERATED_BODY()
	
public:
	virtual FName GetStateName() const override { return TEXT("Stumble"); }
	
	virtual void Reset() override;
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void TickState(float DeltaTime) override;
	
private:
	FTimerHandle StumbleTimerHandle;
	float StumbleDuration{2.0f};
	float CurTime{0.f};
	
	float StumbleStrength{800.f};
}; 
 