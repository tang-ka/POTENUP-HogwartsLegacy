// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianState/GuardianStateBase.h"
#include "Enemy/EnemyFSMComponent.h"

AActor* UGuardianStateBase::GetOwnActor() const
{
	return OwnerFSM ? OwnerFSM->GetOwner() : nullptr;
}

UWorld* UGuardianStateBase::GetWorld() const
{
	if (OwnerFSM)
	{
		return OwnerFSM->GetWorld();
	}
	
	return nullptr;
}
