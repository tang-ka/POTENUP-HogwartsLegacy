// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Protego.h"
#include "Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProtego::AProtego()
{
 	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	SphereMesh->SetRelativeScale3D(FVector(2));
	RootComponent = SphereMesh;
	
	SetActorEnableCollision(false);

	ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (TempMesh.Succeeded())
	{
		SphereMesh->SetStaticMesh(TempMesh.Object);
	}

	ConstructorHelpers::FObjectFinder<UMaterial> TempMat(TEXT("M_Protego'/Game/KHJ/Materials/M_Protego.M_Protego'"));
	if (TempMat.Succeeded())
	{
		SphereMesh->SetMaterial(0, TempMat.Object);
	}

}

// Called when the game starts or when spawned
void AProtego::BeginPlay()
{
	Super::BeginPlay();
	
	player = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());
	
}

// Called every frame
void AProtego::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Duration += DeltaTime;
	if (Duration < 0.5)
	{
		SetActorRelativeScale3D(FVector(2+Duration*1.25));
	}
	if (Duration > 2)
	{
		Destroy();
	}
	
	SetActorLocation(player->GetActorLocation());

}

