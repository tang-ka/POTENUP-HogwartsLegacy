// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/GuardianState/GuardianNonCombatState.h"
#include "Enemy/EnemyFSMComponent.h"
#include "Enemy/Guardian.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGuardianSleepState::OnEnter()
{
	Super::OnEnter();
}

void UGuardianSleepState::OnExit()
{
	Super::OnExit();
}

void UGuardianSleepState::TickState(float DeltaTime)
{
	Super::TickState(DeltaTime);
	
	if (GetOwnerGuardian()->GetCharacterMovement()->IsFalling())
	{
	}
	else
	{
		FTimerHandle AwakeTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(AwakeTimerHandle, [this]()
		{
			GetOwnerFSM()->ChangeState(EEnemyState::Idle);
		}, 1.f, false);
	}
}

// ----------------------------------------------
// ---------------------Idle---------------------

void UGuardianIdleState::OnEnter()
{
	Super::OnEnter();
	
	DecideTarget();
	
	GetOwnerGuardian()->SetMaterialColor(FLinearColor::Yellow);
}

void UGuardianIdleState::OnExit()
{
	Super::OnExit();
	GetOwnerGuardian()->ResetMaterialColor();
}

void UGuardianIdleState::TickState(float DeltaTime)
{
	Super::TickState(DeltaTime);
	
	LookAtTarget(DeltaTime);
	
	IdleTime += DeltaTime;
	if (IdleTime >= IdleDuration)
	{
		GetOwnerFSM()->ChangeState(EEnemyState::Strafe);
	}

	// UE_LOG(LogTemp, Warning, TEXT("Guardian is idling."));
}

void UGuardianIdleState::Reset()
{
	Super::Reset();
	
	IdleTime = 0.f;
}

void UGuardianIdleState::LookAtTarget(float DeltaTime) const
{
	FRotator TargetRotation = (GetOwnerFSM()->GetCurrentTarget()->GetActorLocation() - GetOwnerGuardian()->GetActorLocation()).Rotation();
	GetOwnerGuardian()->SetActorRotation(FMath::RInterpTo(GetOwnerGuardian()->GetActorRotation(), TargetRotation, DeltaTime, 2.f));
}

void UGuardianIdleState::DecideTarget() const
{
	TMap<FName, TObjectPtr<AActor>>* TargetMap = GetOwnerFSM()->GetTargetMap();
	
	FName TargetKey = "";
	
	if (TargetMap->Num() > 1)
	{
		float ClosestDistance = TNumericLimits<float>::Max();
		FVector MyLocation = GetOwnerGuardian()->GetActorLocation();
		for (TPair<FName, TObjectPtr<AActor>> Actor : *TargetMap)
		{
			if (TargetKey.GetStringLength() == 0)
			{
				TargetKey = Actor.Key;
				continue;
			}
		
			FVector TargetLocation = Actor.Value->GetActorLocation();
			float Distance = FVector::Dist(MyLocation, TargetLocation);
			if (Actor.Key == "Player")
			{
				Distance *= 0.7f;
			}
		
			if (ClosestDistance > Distance)
			{
				ClosestDistance = Distance;
				TargetKey = Actor.Key;
			}
		}
	}
	else
	{
		TargetKey = TargetMap->CreateConstIterator()->Key;
	}
	
	GetOwnerFSM()->SetCurrentTarget(TargetKey);
}
