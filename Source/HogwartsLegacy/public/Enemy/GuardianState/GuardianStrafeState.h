// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/GuardianState/GuardianStateBase.h"
#include "GuardianStrafeState.generated.h"

/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UGuardianStrafeState : public UGuardianStateBase
{
	GENERATED_BODY()
	
public:
	virtual FName GetStateName() const override { return TEXT("Strafe"); }
	
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void TickState(float DeltaTime) override;
	
private:
	void ComputeDirectionToTarget();
	void LookAtTarget(float DeltaTime) const;
	void MoveToTarget(float DeltaTime) const;
	
private:
	
	float StrafeSpeed{80.f};
	FVector DirectionToTarget{};
	float ReachDistance{100};
};
