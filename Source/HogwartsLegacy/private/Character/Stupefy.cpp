

#include "Character/Stupefy.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Enemy/Guardian.h" // temp
#include "Enemy/EnemyFSMComponent.h" // temp
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"



AStupefy::AStupefy()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetSphereRadius(8);
	CollisionComp->SetCollisionProfileName(TEXT("Spell"));
	CollisionComp->SetGenerateOverlapEvents(true);

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AStupefy::OnOverlap);
	RootComponent = CollisionComp;
	
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraSystem"));
	NiagaraComp->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> TempComp(TEXT("NiagaraSystem'/Game/KHJ/FX/NS_Stupefy.NS_Stupefy'"));
	if (TempComp.Succeeded())
	{
		NiagaraComp->SetAsset(TempComp.Object);
	}
	
	ExplosionComp = CreateDefaultSubobject<UNiagaraSystem>(TEXT("ExplosionNiagaraSystem"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ExpTempComp(TEXT("ExplosionNiagaraSystem'/Game/KHJ/FX/NS_Stupefy_Explosion.NS_Stupefy_Explosion'"));
	if (ExpTempComp.Succeeded())
	{
		ExplosionComp = ExpTempComp.Object;
	}
	
	Tags.Add(FName("Spell"));
	InitialLifeSpan = 2.5f;
}

void AStupefy::BeginPlay()
{
	Super::BeginPlay();
	
	if (SpellSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SpellSound);
	}
}

void AStupefy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SetActorLocation(UKismetMathLibrary::VInterpTo_Constant(GetActorLocation(), TargetLocation, DeltaTime, Speed), true);
}

void AStupefy::SetTarget(AActor* target)
{
	if (IsValid(target))
	{
		TargetLocation = target->GetActorLocation() + UKismetMathLibrary::MakeVector(0,0,40);
	}
}

void AStupefy::NoTarget()
{
	TargetLocation = this->GetActorLocation() + this->GetActorForwardVector() * Distance;
}

void AStupefy::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto gd = Cast<AGuardian>(OtherActor); // temp
	
	if (gd && gd->IsValidLowLevel())
	{
		gd->GetFSMComponent()->ChangeState(EEnemyState::Stun); // temp
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionComp, SweepResult.TraceStart);
		Destroy();
	}
}