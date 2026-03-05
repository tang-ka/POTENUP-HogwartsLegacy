// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardianAnim.h"

#include "Enemy/EnemyFSMComponent.h"
#include "Enemy/Guardian.h"

void UGuardianAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	OwningGuardian = Cast<AGuardian>(TryGetPawnOwner());
	if (OwningGuardian)
	{
		if (auto* FSMComponent = OwningGuardian->GetFSMComponent())
		{
			FSMComponent->OnStateChanged.AddUObject(this, &UGuardianAnim::HandleStateChanged);
		}
	}
}

void UGuardianAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (OwningGuardian)
	{
		MoveDirection = OwningGuardian->GetAnimWalkDirection();;
	}
}

void UGuardianAnim::HandleStateChanged(EEnemyState NewState)
{
	GuardianState = NewState;
}
