// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Guardian.h"

#include "HogwartsLegacy.h"
#include "Enemy/EnemyFSMComponent.h"
#include "Rendering/RenderCommandPipes.h"

// Sets default values
AGuardian::AGuardian()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 스켈레탈메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> 
	tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/SSH/ThirdPerson_Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
		PRINT_LOG(TEXT("My Log : %s"), TEXT("Enemy"));
	}
	
	FSMComponent = CreateDefaultSubobject<UEnemyFSMComponent>(TEXT("FSMComponent"));
}

void AGuardian::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultColor = GetMesh()->CreateAndSetMaterialInstanceDynamic(0)->K2_GetVectorParameterValue("Paint Tint");
}

void AGuardian::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGuardian::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AGuardian::SetMaterialColor(FLinearColor Color)
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	UMaterialInstanceDynamic* MID = MeshComp->CreateAndSetMaterialInstanceDynamic(0);
	
	MID->SetVectorParameterValue("Paint Tint", Color);
}

void AGuardian::ResetMaterialColor()
{
	SetMaterialColor(DefaultColor);
}

