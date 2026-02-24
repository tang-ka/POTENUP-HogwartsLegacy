// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianState/GuardianChargeAttackState.h"

#include "AIController.h"
#include "Enemy/EnemyFSMComponent.h"
#include "Enemy/Guardian.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGuardianChargeAttackState::OnEnter()
{
	Super::OnEnter();
	
	CurTime = 0;
	Target = GetOwnerFSM()->GetCurrentTarget();
}

void UGuardianChargeAttackState::OnExit()
{
	Super::OnExit();
}

void UGuardianChargeAttackState::TickState(float DeltaTime)
{
	Super::TickState(DeltaTime);
	
	AGuardian* Guardian = GetOwnerGuardian();
	
	float DistSqt = FVector::DistSquared(Guardian->GetActorLocation(), Target->GetActorLocation()); 
	if (DistSqt <= FMath::Square(AttackRange))
	{
		Guardian->GetCharacterMovement()->StopMovementImmediately();
		return;
	}
	
	CurTime += DeltaTime;
	
	if (CurTime >= ChargingTime)
	{
		if (Guardian && Target)
		{
			FVector Direction = Target->GetActorLocation() - Guardian->GetActorLocation();
			Direction.Z = 0.0f;
			Direction.Normalize();
			
			FVector LaunchVelocity = Direction * ChargeStrength;
			
			Guardian->LaunchCharacter(LaunchVelocity, true, true);
		}
	}
}
