// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GuardianStateBase.generated.h"

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
	
	virtual void OnEnter() {}
	virtual void OnExit() {}
	virtual void TickState(float DeltaTime) {}
	// virtual void HandleEvent()
	
	void Init(UEnemyFSMComponent* InOwner) { OwnerFSM = InOwner;}
	
protected:
	UFUNCTION()
	AActor* GetOwnActor() const;
	
	UFUNCTION()
	virtual UWorld* GetWorld() const override;
	
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UEnemyFSMComponent> OwnerFSM;
};
