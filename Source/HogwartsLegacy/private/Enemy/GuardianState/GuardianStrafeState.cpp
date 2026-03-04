// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianState/GuardianStrafeState.h"

#include "AIController.h"
#include "Enemy/EnemyFSMComponent.h"
#include "Enemy/Guardian.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGuardianStrafeState::OnEnter()
{
	Super::OnEnter();
	
	// 머터리얼의 색을 오렌지로 변경 하고 싶다.
	GetOwnerGuardian()->SetMaterialColor(FLinearColor{255.f / 255.f, 111.f / 255.f, 0.f / 255.f});;
	
	GetOwnerGuardian()->SetCharacterMaxWalkSpeed(StrafeSpeed);
	GetOwnerGuardian()->MoveToActor(GetOwnerFSM()->GetCurrentTarget(), ReachDistance);
}

void UGuardianStrafeState::OnExit()
{
	Super::OnExit();
	GetOwnerGuardian()->ResetMaterialColor();
	
	GetOwnerGuardian()->StopMovementImmediately();
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
