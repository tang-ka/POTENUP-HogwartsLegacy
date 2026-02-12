// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Guardian.h"

#include "HogwartsLegacy.h"

// Sets default values
AGuardian::AGuardian()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 스켈레탈메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> 
	tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
		PRINT_LOG(TEXT("My Log : %s"), TEXT("Enemy"));
	}
}

void AGuardian::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGuardian::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGuardian::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

