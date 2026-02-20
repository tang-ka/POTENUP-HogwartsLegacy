// Fill out your copyright notice in the Description page of Project Settings.

#include "SpellBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

// 생성자 (기본 설정과 컴포넌트 생성을 여기서 한꺼번에 합니다)
ASpellBase::ASpellBase()
{
	// 틱을 켤지 말지 설정 (발사체는 보통 Tick보다 ProjectileMovement가 처리하므로 꺼도 되지만, 일단 둡니다)
	PrimaryActorTick.bCanEverTick = true;

	// 1. 충돌체 생성 및 루트 설정
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = CollisionComponent;
	CollisionComponent->InitSphereRadius(15.0f);

	// 2. 발사체 무브먼트 생성
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
    
	// 설계하신 변수를 실제 컴포넌트 속도에 연결
	ProjectileMovement->InitialSpeed = SpellSpeed;
	ProjectileMovement->MaxSpeed = SpellSpeed;
    
	// 중력 0 (마법이니까 직선으로 쭉!)
	ProjectileMovement->ProjectileGravityScale = 0.0f;
    
	// 생성 후 3초 뒤에 자동으로 메모리에서 삭제
	InitialLifeSpan = 3.0f;
}

// 게임 시작 시 호출
void ASpellBase::BeginPlay()
{
	Super::BeginPlay();
}

// 매 프레임 호출
void ASpellBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}