// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyFSMComponent.h"

#include "HogwartsLegacy.h"
#include "Enemy/GuardianState/GuardianStateBase.h"

UEnemyFSMComponent::UEnemyFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UEnemyFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UEnemyFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurState->TickState(DeltaTime);
	// ...
}

void UEnemyFSMComponent::ChangeState(UGuardianStateBase* NewState)
{
	// Prevent re-entrancy
	if (!NewState || NewState == CurState || bIsChanging)
	{
		return;
	}
	
	bIsChanging = true;
	
	const FName From = CurState ? CurState->GetStateName() : TEXT("None");
	const FName To = NewState ? NewState->GetStateName() : TEXT("None");
	PRINT_LOG(TEXT("State Change: %s -> %s"), *From.ToString(), *To.ToString());
	
	if (CurState)
	{
		CurState->OnExit();
		PRINT_LOG(TEXT("State Exit: %s"), *CurState->GetStateName().ToString());
	}
	
	CurState = NewState;
	
	if (CurState)
	{
		CurState->OnEnter();
		PRINT_LOG(TEXT("State Enter: %s"), *CurState->GetStateName().ToString());
	}
}

