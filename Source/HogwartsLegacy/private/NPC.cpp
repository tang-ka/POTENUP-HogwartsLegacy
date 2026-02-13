// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC.h"
#include "AIController.h"

// Sets default values
ANPC::ANPC()
{
	// Set this character to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	// --- 기본 상태를 Idle로 설정 ---
	CurrentState = ENPCState::Idle;
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// --- 상태 전환 함수 구현부 ---
void ANPC::SetNPCState(ENPCState NewState)
{
	CurrentState = NewState;
}

// --- 타겟 초기화 및 시선 해제 함수 ---
void ANPC::ClearTargetEnemy()
{
	// 1. 타겟 변수 비우기 (가디언 정보 삭제)
	TargetEnemy = nullptr;

	// 2. AI 컨트롤러를 가져와서 시선 고정(Focus) 풀기
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->ClearFocus(EAIFocusPriority::Gameplay);
	}
    
	// 3. 상태를 다시 Idle로 돌려놓기 (변수명 CurrentState로 통일)
	CurrentState = ENPCState::Idle;
}