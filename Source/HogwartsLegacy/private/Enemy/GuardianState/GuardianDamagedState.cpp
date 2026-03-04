// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianState/GuardianDamagedState.h"

#include "Enemy/EnemyFSMComponent.h"
#include "Enemy/Guardian.h"

void UGuardianDamagedState::Reset()
{
	Super::Reset();
	CurTime = 0.f;
}

void UGuardianDamagedState::OnEnter()
{
	Super::OnEnter();
	
	GetOwnerGuardian()->SetMaterialColor(FLinearColor::Red);
}

void UGuardianDamagedState::OnExit()
{
	Super::OnExit();
	
	GetOwnerGuardian()->ResetMaterialColor();
}

void UGuardianDamagedState::TickState(float DeltaTime)
{
	Super::TickState(DeltaTime);
	
	CurTime += DeltaTime;
	if (CurTime >= DamagedDuration)
	{
		GetOwnerFSM()->ChangeState(EEnemyState::Idle);
	}
}
