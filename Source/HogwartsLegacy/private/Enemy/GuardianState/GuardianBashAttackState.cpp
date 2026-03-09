// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianState/GuardianBashAttackState.h"

#include "Enemy/EnemyFSMComponent.h"
#include "Enemy/Guardian.h"

void UGuardianBashAttackState::Reset()
{
	Super::Reset();
	
	CurTime = 0.f;
	bIsBashing = false;
}

void UGuardianBashAttackState::OnEnter()
{
	Super::OnEnter();
	
	Target = GetOwnerFSM()->GetCurrentTarget();
	// GetOwnerFSM()->bIsAttacking = true;
}

void UGuardianBashAttackState::OnExit()
{
	Super::OnExit();
	GetOwnerFSM()->OnAttackFinished.Broadcast();
}

void UGuardianBashAttackState::TickState(float DeltaTime)
{
	Super::TickState(DeltaTime);
	
	if (!bIsBashing)
	{
		CurTime += DeltaTime;
		if (CurTime >= ChargingTime)
		{
			BashAttack();
		}
	}
	else
	{
		BashAttack();
		bIsBashing = true;
	}
}

void UGuardianBashAttackState::BashAttack()
{
	auto* Guardian = GetOwnerGuardian();
	
	if (IsValid(Guardian) && IsValid(Target))
	{
		bIsBashing = true;
		Guardian->ExecuteBashAttack(Target, BashStrength);
	}
	
}
