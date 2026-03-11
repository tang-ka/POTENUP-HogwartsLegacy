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
	// GetOwnerGuardian()->SetMaterialColor(FLinearColor{255.f / 255.f, 111.f / 255.f, 0.f / 255.f});;
	
	GetOwnerGuardian()->SetCharacterMaxWalkSpeed(StrafeSpeed);
	ComputeDirectionToTarget();
	float DistanceToTarget = GetDistanceToTarget();
	
	// 개 가까움
	if (DistanceToTarget < ReachDistance - CloseOffset)
	{
		FVector BackwardDirection = -DirectionToTarget; // 타겟에서 멀어지는 방향
		GetOwnerGuardian()->MoveToLocation(GetOwnerGuardian()->GetActorLocation() + BackwardDirection * 200.f);
	}
	// 적당한 거리
	else if (DistanceToTarget <= ReachDistance + FarOffset)
	{
		float DirectionSign = FMath::RandBool() ? 1.f : -1.f; // 왼쪽 또는 오른쪽으로 무작위 방향 결정
		FVector StrafeDirection = FVector::CrossProduct(DirectionToTarget, FVector::UpVector) * DirectionSign; // 타겟을 중심으로 수직인 방향 계산
		GetOwnerGuardian()->MoveToLocation(GetOwnerGuardian()->GetActorLocation() + StrafeDirection * 150.f);
	}
	// 개 멀음
	else 
	{
		GetOwnerGuardian()->MoveToActor(GetOwnerFSM()->GetCurrentTarget(), ReachDistance);
	}
}

float UGuardianStrafeState::GetDistanceToTarget() const
{
	return FVector::Dist(GetOwnerGuardian()->GetActorLocation(), GetOwnerFSM()->GetCurrentTarget()->GetActorLocation());
}

// void UGuardianStrafeState::OnExit()
// {
// 	Super::OnExit();
// 	// GetOwnerGuardian()->ResetMaterialColor();
// 	
// 	GetOwnerGuardian()->StopMovementImmediately();
// }

// void UGuardianStrafeState::TickState(float DeltaTime)
// {
// 	Super::TickState(DeltaTime);
// 	
// 	// ComputeDirectionToTarget();
// 	// LookAtTarget(DeltaTime);	 
// 	// MoveToTarget(DeltaTime);
// }

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
