// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSMComponent.generated.h"

class UGuardianStateBase;

UENUM()
enum class EEnemyState : uint8
{
	Sleep, Idle,
	Strafe,
	ChargeAttack, BashAttack,
	Stumble,
	Damaged, Stun, Dead
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOGWARTSLEGACY_API UEnemyFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyFSMComponent();

public:	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
#pragma region Getters
	FORCEINLINE int32 GetHP() const { return HP; }
	FORCEINLINE int32 GetMaxHP() const { return MaxHP; }
	
	UGuardianStateBase* GetStateByType(EEnemyState StateType) const;
#pragma endregion 

	UFUNCTION(BlueprintCallable)
	void ChangeState(EEnemyState NewStateType);
	
protected:
	UFUNCTION()
	void Init();
	
	UFUNCTION()
	void InitStates();

private:
	UPROPERTY(EditAnywhere, Category="GuardianFSM|Stats")
	int32 MaxHP{100};

	UPROPERTY(EditAnywhere, Category="GuardianFSM|Stats")
	int32 HP{MaxHP};

	UPROPERTY(Transient)
	TObjectPtr<UGuardianStateBase> CurState;
	
	UPROPERTY(Transient, VisibleAnywhere, Category="GuardianFSM|State")
	EEnemyState CurStateType;
	
#pragma region State Instance 
	UPROPERTY(Transient)
	TMap<EEnemyState, TObjectPtr<UGuardianStateBase>> StateMap;
	
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
