#include "NPC.h"
#include "AIController.h"
#include "GameFramework/Character.h"

// 1. 생성자
ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentState = ENPCState::Idle;
}

// 2. BeginPlay
void ANPC::BeginPlay()
{
	Super::BeginPlay();
}

// 3. Tick
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 4. 입력 설정
void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// 5. 상태 전환 함수
void ANPC::SetNPCState(ENPCState NewState)
{
	CurrentState = NewState;
}

// 6. 타겟 초기화 및 시선 해제 (담당자님 기존 로직 완벽 반영)
void ANPC::ClearTargetEnemy()
{
	TargetEnemy = nullptr;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->ClearFocus(EAIFocusPriority::Gameplay);
	}
    
	CurrentState = ENPCState::Idle;
}

// 7. 반격 로직 (주황색 선 뚫어주는 핵심 엔진)
float ANPC::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageCauser != nullptr)
	{
		TargetEnemy = DamageCauser;
	}

	// 상태를 Combat으로 변경하여 블루프린트 Switch 노드 가동
	SetNPCState(ENPCState::Combat);

	return ActualDamage;
}