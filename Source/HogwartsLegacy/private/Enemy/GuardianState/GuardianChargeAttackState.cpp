// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianState/GuardianChargeAttackState.h"

#include "AIController.h"
#include "HogwartsLegacy.h"
#include "Enemy/EnemyFSMComponent.h"
#include "Enemy/Guardian.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGuardianChargeAttackState::Reset()
{
	Super::Reset();
	
	CurTime = 0;
	bIsDashing = false;
	bIsReachedTarget = false;
}

void UGuardianChargeAttackState::OnEnter()
{
	Super::OnEnter();
	
	Target = GetOwnerFSM()->GetCurrentTarget();
	// GetOwnerFSM()->bIsAttacking = true;
}

void UGuardianChargeAttackState::OnExit()
{
	Super::OnExit();
	GetOwnerFSM()->OnAttackFinished.Broadcast();
}

void UGuardianChargeAttackState::TickState(float DeltaTime)
{
	Super::TickState(DeltaTime);
	
	if (!bIsDashing)
	{
		CurTime += DeltaTime;
		if (CurTime >= ChargingTime)
		{
			ChargeAttack();
		}
	}
	else if (!bIsReachedTarget)
	{
		if (IsTargetInRange())
		{
			bIsReachedTarget = true;
			
			// 위치 보정
			GetOwnerGuardian()->SnapToAttackLocation(Target, AttackRange);
			
			// 공격 실행
			GetOwnerGuardian()->ExecuteMeleeAttack();
		}
	}
	else
	{
		// PRINT_LOG(TEXT("Charge Attack Reached Target"));
	}
}

bool UGuardianChargeAttackState::IsTargetInRange() const
{
	AGuardian* Guardian = GetOwnerGuardian();
	
	float DistSqt = FVector::DistSquaredXY(Guardian->GetActorLocation(), Target->GetActorLocation()); 
	return DistSqt <= FMath::Square(AttackRange);
}

void UGuardianChargeAttackState::ChargeAttack()
{
	AGuardian* Guardian = GetOwnerGuardian();
	
	if (Guardian && Target)
	{
		bIsDashing = true;
		Guardian->ExecuteChargeToTarget(Target, ChargeStrength);
	}
}
