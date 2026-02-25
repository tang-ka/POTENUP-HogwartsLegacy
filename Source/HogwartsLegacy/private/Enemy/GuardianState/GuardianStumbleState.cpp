// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianState/GuardianStumbleState.h"

#include "Enemy/Guardian.h"

void UGuardianStumbleState::Reset()
{
	Super::Reset();
	
	CurTime = 0.f;
}

void UGuardianStumbleState::OnEnter()
{
	Super::OnEnter();
	
	GetOwnerGuardian()->SetMaterialColor(FLinearColor::Blue);
	
	GetOwnerGuardian()->ExecuteStumble(StumbleStrength);
}

void UGuardianStumbleState::OnExit()
{
	Super::OnExit();
	
	GetOwnerGuardian()->ResetMaterialColor();
	
}

void UGuardianStumbleState::TickState(float DeltaTime)
{
	Super::TickState(DeltaTime);
}
