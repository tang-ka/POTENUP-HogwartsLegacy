// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianState/GuardianNonCombatState.h"

void UGuardianSleepState::OnEnter()
{
	Super::OnEnter();
}

void UGuardianSleepState::OnExit()
{
	Super::OnExit();
}

void UGuardianSleepState::TickState(float DeltaTime)
{
	Super::TickState(DeltaTime);
	
	UE_LOG(LogTemp, Warning, TEXT("Guardian is sleeping."));
}

void UGuardianIdleState::OnEnter()
{
	Super::OnEnter();
}

void UGuardianIdleState::OnExit()
{
	Super::OnExit();
}

void UGuardianIdleState::TickState(float DeltaTime)
{
	Super::TickState(DeltaTime);
	
	UE_LOG(LogTemp, Warning, TEXT("Guardian is idling."));
}
