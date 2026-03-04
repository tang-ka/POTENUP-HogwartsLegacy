// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianState/GuardianDeadState.h"

#include "Enemy/Guardian.h"

void UGuardianDeadState::OnEnter()
{
	Super::OnEnter();
	
	FTimerHandle DestroyTimerHandle;
	TWeakObjectPtr<ThisClass> WeakThis(this);

	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, [WeakThis]()
	{
		// 실행 시점에 객체가 아직 살아있는지 확인합니다.
		if (WeakThis.IsValid())
		{
			if (AGuardian* Guardian = WeakThis->GetOwnerGuardian())
			{
				Guardian->Destroy();
			}
		}
	}, 2.0f, false);
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
