// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSMComponent.generated.h"

class AGuardian;
class AAIController;
class UGuardianStateBase;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Sleep, Idle,
	Strafe,
	ChargeAttack, BashAttack,
	Stumble,
	Damaged, Stun, Dead
};

DECLARE_MULTICAST_DELEGATE(FOnEnemyDead);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeState, EEnemyState);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEnemyDamaged, float, float);
DECLARE_MULTICAST_DELEGATE(FOnAttackFinished);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOGWARTSLEGACY_API UEnemyFSMComponent : public UActorComponent
{
	GENERATED_BODY()
	
	friend class UGuardianStateBase;
	friend class UGuardianBashAttackState;
	friend class UGuardianChargeAttackState;

public:	
	UEnemyFSMComponent();

public:	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
#pragma region Getters
	AAIController* GetAIController() const { return AIController; }
	
	UFUNCTION(BlueprintCallable)
	EEnemyState GetCurrentStateType() const { return CurStateType; }
	UGuardianStateBase* GetStateByType(EEnemyState StateType) const;
	TMap<FName, TObjectPtr<AActor>>* GetTargetMap() { return &TargetMap; }
	AActor* GetCurrentTarget() const { return CurrentTarget; }
#pragma endregion 
	
	UFUNCTION(BlueprintCallable)
	void AddTarget(FName TargetKey, AActor* TargetActor);
	
	UFUNCTION(BlueprintCallable)
	void SetCurrentTarget(FName TargetKey);

	UFUNCTION(BlueprintCallable)
	void ChangeState(EEnemyState NewStateType);
	
#pragma region Request State Change
	UFUNCTION(BlueprintCallable)
	void RequestAttack();
	
	UFUNCTION(BlueprintCallable)
	void FinishAttack(bool bSuccess);
	
	UFUNCTION(BLueprintCallable)
	void RequestDead();
	
	UFUNCTION(BlueprintCallable)
	void RequestDamaged(float DamageAmount, float RemainingHP);
#pragma endregion

	bool IsDead() const { return CurStateType == EEnemyState::Dead; }
	
protected:
	UFUNCTION()
	void Init();
	
	UFUNCTION()
	void InitStates();
	
public:
	FOnEnemyDead OnEnemyDead;
	FOnChangeState OnStateChanged;
	FOnEnemyDamaged OnEnemyDamaged;
	FOnAttackFinished OnAttackFinished;
	
private:
	UPROPERTY()
	TObjectPtr<AGuardian> Me;
	
	UPROPERTY()
	TObjectPtr<AAIController> AIController;
	
	UPROPERTY(VisibleAnyWhere, Category="Target")
	TMap<FName, TObjectPtr<AActor>> TargetMap;
	
	UPROPERTY(VisibleAnywhere, Category="Target")
	TObjectPtr<AActor> CurrentTarget;

	UPROPERTY(Transient)
	TObjectPtr<UGuardianStateBase> CurState;
	
	UPROPERTY(Transient, VisibleAnywhere, Category="State")
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
