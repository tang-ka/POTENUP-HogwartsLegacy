// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianState/GuardianStunState.h"

#include "Enemy/EnemyFSMComponent.h"
#include "Enemy/Guardian.h"

void UGuardianStunState::Reset()
{
	Super::Reset();
	
	CurTime = 0.f;
}

// void UGuardianStunState::OnEnter()
// {
// 	Super::OnEnter();
// 	
// 	GetOwnerGuardian()->SetMaterialColor(FLinearColor::Green);
// }
//
// void UGuardianStunState::OnExit()
// {
// 	Super::OnExit();
// 	
// 	GetOwnerGuardian()->ResetMaterialColor();
// }

void UGuardianStunState::TickState(float DeltaTime)
{
	Super::TickState(DeltaTime);
	
	CurTime += DeltaTime;
	if (CurTime >= StunDuration)
	{
		GetOwnerFSM()->ChangeState(EEnemyState::Idle);
	}
}
