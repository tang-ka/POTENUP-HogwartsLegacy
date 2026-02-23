// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianState/GuardianStateBase.h"
#include "Enemy/EnemyFSMComponent.h"
#include "Enemy/Guardian.h"

void UGuardianStateBase::OnEnter()
{
	Reset();
}

void UGuardianStateBase::InitOwner(UEnemyFSMComponent* InFSMComp)
{
	OwnerFSM = InFSMComp;
	OwnerGuardian = Cast<AGuardian>(InFSMComp->GetOwner());
}
