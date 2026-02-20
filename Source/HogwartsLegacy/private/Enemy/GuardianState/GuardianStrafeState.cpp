// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianState/GuardianStrafeState.h"

#include "Enemy/EnemyFSMComponent.h"
#include "Enemy/Guardian.h"

void UGuardianStrafeState::OnEnter()
{
	Super::OnEnter();
	
	AGuardian* OwnerGuardian = Cast<AGuardian>(GetOwnerActor());
	OwnerGuardian->SetMaterialColor(FLinearColor::Red);
}

void UGuardianStrafeState::OnExit()
{
	Super::OnExit();
	AGuardian* OwnerGuardian = Cast<AGuardian>(GetOwnerActor());
	OwnerGuardian->ResetMaterialColor();
}

void UGuardianStrafeState::TickState(float DeltaTime)
{
	Super::TickState(DeltaTime);
	
	ComputeDirectionToTarget();
	LookAtTarget(DeltaTime);	
	MoveToTarget(DeltaTime);
}

void UGuardianStrafeState::ComputeDirectionToTarget()
{
	DirectionToTarget = GetOwnerFSM()->GetCurrentTarget()->GetActorLocation() - GetOwnerActor()->GetActorLocation();
	DirectionToTarget.Z = 0.f; // Ignore vertical difference for strafing
	DirectionToTarget.Normalize();
}

void UGuardianStrafeState::LookAtTarget(float DeltaTime) const
{
	FRotator LookAtRotation = DirectionToTarget.Rotation();
	FRotator CurrentRotation = GetOwnerActor()->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, 20.f);
	GetOwnerActor()->SetActorRotation(NewRotation);
}

void UGuardianStrafeState::MoveToTarget(float DeltaTime) const
{
	FVector NewLocation = GetOwnerActor()->GetActorLocation() + DirectionToTarget * StrafeSpeed * DeltaTime;
	GetOwnerActor()->SetActorLocation(NewLocation);
}
