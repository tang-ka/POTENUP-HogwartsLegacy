// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSMComponent.generated.h"

class UGuardianStateBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOGWARTSLEGACY_API UEnemyFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyFSMComponent();

public:	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	void ChangeState(UGuardianStateBase* NewState);
	
public:
#pragma region Getters
	int32 GetHP() const { return HP; }
	int32 GetMaxHP() const { return MaxHP; }
	
	UGuardianStateBase* GetCurState() const { return CurState; }
	
	UGuardianStateBase* GetSleepState() const { return SleepState; }
	UGuardianStateBase* GetIdleState() const { return IdleState; }
	
	UGuardianStateBase* GetStrafeState() const { return StrafeState; }
	
	UGuardianStateBase* GetChargeAttackState() const { return ChargeAttackState; }
	UGuardianStateBase* GetBashAttackState() const { return BashAttackState; }
	
	UGuardianStateBase* GetStumbleState() const { return StumbleState; }
	
	UGuardianStateBase* GetDamagedState() const { return DamagedState; }
	UGuardianStateBase* GetStunState() const { return StunState; }
	UGuardianStateBase* GetDeadState() const { return DeadState; }
#pragma endregion 
	
private:
	UPROPERTY(EditAnywhere, Category="GuadrdianFSM|Stats")
	int32 MaxHP{100};

	UPROPERTY(EditAnywhere, Category="GuadrdianFSM|Stats")
	int32 HP{MaxHP};
	
	UPROPERTY(EditAnywhere, Category="GuadrianFSM|State")
	FName CurStateName{};
	
	UPROPERTY(Transient)
	TObjectPtr<UGuardianStateBase> CurState;
	
#pragma region State Instance 
	// Non Combat
	UPROPERTY(Transient) TObjectPtr<UGuardianStateBase> SleepState;
	UPROPERTY(Transient) TObjectPtr<UGuardianStateBase> IdleState;
	
	// Combat Readiness
	UPROPERTY(Transient) TObjectPtr<UGuardianStateBase> StrafeState;
	
	// Offense
	UPROPERTY(Transient) TObjectPtr<UGuardianStateBase> ChargeAttackState;
	UPROPERTY(Transient) TObjectPtr<UGuardianStateBase> BashAttackState;
	
	// Special
	UPROPERTY(Transient) TObjectPtr<UGuardianStateBase> StumbleState;
	
	// Reaction
	UPROPERTY(Transient) TObjectPtr<UGuardianStateBase> DamagedState;
	UPROPERTY(Transient) TObjectPtr<UGuardianStateBase> StunState;
	UPROPERTY(Transient) TObjectPtr<UGuardianStateBase> DeadState;
#pragma endregion
	
	bool bIsChanging{false};
};
