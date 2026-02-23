// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GuardianStateBase.generated.h"

class AGuardian;
class UEnemyFSMComponent;
/**
 * 
 */
UCLASS(Abstract)
class HOGWARTSLEGACY_API UGuardianStateBase : public UObject
{
	GENERATED_BODY()
	
public:
	virtual FName GetStateName() const { return TEXT("StateBase"); }
	
	virtual void OnEnter();
	virtual void OnExit() {}
	virtual void TickState(float DeltaTime) {}
	// virtual void HandleEvent()
	
	void InitOwner(UEnemyFSMComponent* InFSMComp);

protected:
	// Override this to reset any state-specific variables when entering the state
	virtual void Reset() {};
	
protected:
	AGuardian* GetOwnerGuardian() const { return OwnerGuardian; }
	UEnemyFSMComponent* GetOwnerFSM() const { return OwnerFSM; }

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UEnemyFSMComponent> OwnerFSM;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<AGuardian> OwnerGuardian;
};
