// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Lumos.h"
#include "NiagaraFunctionLibrary.h"


// Sets default values
ALumos::ALumos()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StartNiagara = CreateDefaultSubobject<UNiagaraSystem>(TEXT("NiagaraSystem"));

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> TempComp(TEXT("NiagaraSystem'/Game/KHJ/FX/NS_Lumos.NS_Lumos'"));
	if (TempComp.Succeeded())
	{
		  StartNiagara = TempComp.Object;
	}
	
	SpellSphereComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpellSphere"));
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (TempMesh.Succeeded())
	{
		SpellSphereComp->SetStaticMesh(TempMesh.Object);
	}
	
	ConstructorHelpers::FObjectFinder<UMaterial> TempMat3(TEXT("LumosMat'/Game/KHJ/Materials/M_Lumos.M_Lumos'"));
	if (TempMat3.Succeeded())
	{
		SpellSphereComp->SetMaterial(0, TempMat3.Object);
	}
	
	SpellSphereComp->SetWorldScale3D(FVector(0.025f));
	SpellSphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	Tags.Add(FName("Spell"));
}

// Called when the game starts or when spawned
void ALumos::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnedNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), StartNiagara, this->GetActorLocation());
}

// Called every frame
void ALumos::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SetActorLocation(Caster->GetComponentLocation());
	SpawnedNiagara->SetWorldLocation(Caster->GetComponentLocation());
}

void ALumos::BeginDestroy()
{
	Super::BeginDestroy();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), StartNiagara, this->GetActorLocation());
}

void ALumos::SetLocation(USceneComponent* WandTip)
{
	Caster = WandTip;
}

