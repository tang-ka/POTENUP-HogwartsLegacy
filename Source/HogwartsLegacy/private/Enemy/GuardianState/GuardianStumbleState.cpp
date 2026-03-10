// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianState/GuardianStumbleState.h"

#include "Enemy/EnemyFSMComponent.h"
#include "Enemy/Guardian.h"

void UGuardianStumbleState::Reset()
{
	Super::Reset();
	
	CurTime = 0.f;
}

void UGuardianStumbleState::OnEnter()
{
	Super::OnEnter();
	
	// GetOwnerGuardian()->SetMaterialColor(FLinearColor::Blue);
	
	GetOwnerGuardian()->ExecuteStumble(StumbleStrength);
	
	GetWorld()->GetTimerManager().SetTimer(StumbleTimerHandle, [this]()
	{		
		GetOwnerFSM()->ChangeState(EEnemyState::Idle);
	}, 
	StumbleDuration, false);
}

void UGuardianStumbleState::OnExit()
{
	Super::OnExit();
	
	// GetOwnerGuardian()->ResetMaterialColor();
	GetWorld()->GetTimerManager().ClearTimer(StumbleTimerHandle);
}
