// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianState/GuardianDeadState.h"

#include "Enemy/Guardian.h"

void UGuardianDeadState::OnEnter()
{
	Super::OnEnter();
}

void UGuardianDeadState::OnExit()
{
	Super::OnExit();
}

void UGuardianDeadState::TickState(float DeltaTime)
{
	Super::TickState(DeltaTime);
	
	// 제자리에서 회전하고 싶다.
	FRotator NewRotation = GetOwnerGuardian()->GetActorRotation();
	NewRotation.Yaw += DeltaTime * 720.0f; // 회전 속도 조정
	GetOwnerGuardian()->SetActorRotation(NewRotation);
}
