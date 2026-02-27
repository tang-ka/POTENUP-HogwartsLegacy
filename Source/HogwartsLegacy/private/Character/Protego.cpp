// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Protego.h"


// Sets default values
AProtego::AProtego()
{
 	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
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
	
	Tags.Add(FName("Spell"));
	InitialLifeSpan = 2.0f;
	
}

// Called when the game starts or when spawned
void AProtego::BeginPlay()
{
	Super::BeginPlay();
	
	Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	
}

// Called every frame
void AProtego::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Duration += DeltaTime;
	if (Duration < 0.2)
	{
		SetActorRelativeScale3D(FVector(2+Duration*2));
	}
	
	SetActorLocation(Player->GetActorLocation());

}

