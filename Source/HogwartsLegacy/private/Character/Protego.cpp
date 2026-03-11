

#include "Character/Protego.h"

#include "Kismet/GameplayStatics.h"


AProtego::AProtego()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	RootComponent = SphereMesh;
	
	SetActorEnableCollision(false);
	

	static ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (TempMesh.Succeeded())
	{
		SphereMesh->SetStaticMesh(TempMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> TempMat(TEXT("M_Protego'/Game/KHJ/Materials/M_Protego.M_Protego'"));
	if (TempMat.Succeeded())
	{
		SphereMesh->SetMaterial(0, TempMat.Object);
	}
	
	Tags.Add(FName("Spell"));
	InitialLifeSpan = 1.5f;
	
}

void AProtego::BeginPlay()
{
	Super::BeginPlay();
	
	Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	
	if (SpellSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SpellSound);
	}
	
}

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

