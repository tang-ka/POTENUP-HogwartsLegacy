// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianState/GuardianStrafeState.h"

#include "AIController.h"
#include "Enemy/EnemyFSMComponent.h"
#include "Enemy/Guardian.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGuardianStrafeState::OnEnter()
{
	Super::OnEnter();
	
	GetOwnerGuardian()->SetMaterialColor(FLinearColor::Red);
	
	GetOwnerGuardian()->GetCharacterMovement()->MaxWalkSpeed = StrafeSpeed;
	GetOwnerFSM()->GetAIController()->MoveToActor(GetOwnerFSM()->GetCurrentTarget(), ReachDistance);
}

void UGuardianStrafeState::OnExit()
{
	Super::OnExit();
	GetOwnerGuardian()->ResetMaterialColor();
	
	GetOwnerFSM()->GetAIController()->StopMovement();
}

void UGuardianStrafeState::TickState(float DeltaTime)
{
	Super::TickState(DeltaTime);
	
	// ComputeDirectionToTarget();
	// LookAtTarget(DeltaTime);	
	// MoveToTarget(DeltaTime);
}

void UGuardianStrafeState::ComputeDirectionToTarget()
{
	DirectionToTarget = GetOwnerFSM()->GetCurrentTarget()->GetActorLocation() - GetOwnerGuardian()->GetActorLocation();
	DirectionToTarget.Z = 0.f; // Ignore vertical difference for strafing
	DirectionToTarget.Normalize();
}

void UGuardianStrafeState::LookAtTarget(float DeltaTime) const
{
	FRotator LookAtRotation = DirectionToTarget.Rotation();
	FRotator CurrentRotation = GetOwnerGuardian()->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, 20.f);
	GetOwnerGuardian()->SetActorRotation(NewRotation);
}

void UGuardianStrafeState::MoveToTarget(float DeltaTime) const
{
	FVector NewLocation = GetOwnerGuardian()->GetActorLocation() + DirectionToTarget * StrafeSpeed * DeltaTime;
	GetOwnerGuardian()->SetActorLocation(NewLocation);
}
