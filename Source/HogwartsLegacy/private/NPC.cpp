#include "NPC.h"
#include "AIController.h"
#include "GameFramework/Character.h"

ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentState = ENPCState::Idle;
	Speed = 0.0f;
	bIsCombat = false;
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();
}

void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
	Speed = GetVelocity().Size();
}

void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANPC::SetNPCState(ENPCState NewState)
{
	CurrentState = NewState;
}

void ANPC::ClearTargetEnemy()
{
	TargetEnemy = nullptr;
	bIsCombat = false; // 타겟 클리어 시 전투 모드 해제

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->ClearFocus(EAIFocusPriority::Gameplay);
	}
    
	CurrentState = ENPCState::Idle;
}

float ANPC::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageCauser != nullptr)
	{
		TargetEnemy = DamageCauser;
		bIsCombat = true; // 데미지 입으면 전투 모드 활성화
	}

	SetNPCState(ENPCState::Combat);

	return ActualDamage;
}