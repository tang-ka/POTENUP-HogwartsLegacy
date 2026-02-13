// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyFSMComponent.h"

#include "HogwartsLegacy.h"
#include "Enemy/GuardianState/GuardianBashAttackState.h"
#include "Enemy/GuardianState/GuardianChargeAttackState.h"
#include "Enemy/GuardianState/GuardianDamagedState.h"
#include "Enemy/GuardianState/GuardianDeadState.h"
#include "Enemy/GuardianState/GuardianNonCombatState.h"
#include "Enemy/GuardianState/GuardianStateBase.h"
#include "Enemy/GuardianState/GuardianStrafeState.h"
#include "Enemy/GuardianState/GuardianStumbleState.h"
#include "Enemy/GuardianState/GuardianStunState.h"

UEnemyFSMComponent::UEnemyFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	InitStates();
	
	Init();
}

void UEnemyFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurState->TickState(DeltaTime);
	// ...
}

UGuardianStateBase* UEnemyFSMComponent::GetStateByType(EEnemyState StateType) const
{
	return StateMap.Contains(StateType) ? StateMap[StateType] : nullptr;
}

void UEnemyFSMComponent::Init()
{
	ChangeState(EEnemyState::Sleep);
}

void UEnemyFSMComponent::ChangeState(EEnemyState NewStateType)
{
	// Prevent re-entrancy
	if (bIsChanging)
	{
		return;
	}

	if (NewStateType == CurStateType && CurState)
	{
		return;
	}
	
	bIsChanging = true;
	
	UGuardianStateBase* NewState = GetStateByType(NewStateType);
	if (!NewState)
	{
		PRINT_LOG(TEXT("Invalid state type: %d"), static_cast<int32>(NewStateType));
		bIsChanging = false;
		return;
	}
	
	const FName FromName = CurState ? CurState->GetStateName() : TEXT("None");
	const FName ToName = NewState->GetStateName();
	PRINT_LOG(TEXT("State Change: %s -> %s"), *FromName.ToString(), *ToName.ToString());
	
	// 1. Exit the current state
	if (CurState)
	{
		CurState->OnExit();
		PRINT_LOG(TEXT("State Exit: %s"), *CurState->GetStateName().ToString());
	}
	
	// 2. Switch to the new state
	CurState = GetStateByType(NewStateType);
	CurStateType = NewStateType;
	
	// 3. Enter the new state
	CurState->OnEnter();
	PRINT_LOG(TEXT("State Enter: %s"), *CurState->GetStateName().ToString());
	
	// 4. Reset the flag
	bIsChanging = false;
}

void UEnemyFSMComponent::InitStates()
{
	if (!StateMap.IsEmpty())
	{
		return;
	}
	
	// Non Combat
	SleepState = NewObject<UGuardianSleepState>(this);
	IdleState = NewObject<UGuardianIdleState>(this);
	// Combat Readiness
	StrafeState = NewObject<UGuardianStrafeState>(this);
	// Offense
	ChargeAttackState = NewObject<UGuardianChargeAttackState>(this);
	BashAttackState = NewObject<UGuardianBashAttackState>(this);
	// Special
	StumbleState = NewObject<UGuardianStumbleState>(this);
	// Reaction
	DamagedState = NewObject<UGuardianDamagedState>(this);
	StunState = NewObject<UGuardianStunState>(this);
	DeadState = NewObject<UGuardianDeadState>(this);
	
	// Non Combat
	StateMap.Add(EEnemyState::Sleep, SleepState);
	StateMap.Add(EEnemyState::Idle, IdleState);
	// Combat Readiness
	StateMap.Add(EEnemyState::Strafe, StrafeState);
	// Offense
	StateMap.Add(EEnemyState::ChargeAttack, ChargeAttackState);
	StateMap.Add(EEnemyState::BashAttack, BashAttackState);
	// Special
	StateMap.Add(EEnemyState::Stumble, StumbleState);
	// Reaction
	StateMap.Add(EEnemyState::Damaged, DamagedState);
	StateMap.Add(EEnemyState::Stun, StunState);
	StateMap.Add(EEnemyState::Dead, DeadState);
	
	PRINT_LOG(TEXT("Initialized %d states."), StateMap.Num());
}

