// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Guardian.h"

#include "AIController.h"
#include "HogwartsLegacy.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Enemy/EnemyFSMComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/DamageableHelpers.h"
#include "Rendering/RenderCommandPipes.h"

// Sets default values
AGuardian::AGuardian()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 스켈레탈메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh>
		tempMesh(TEXT(
			"/Script/Engine.SkeletalMesh'/Game/SSH/ThirdPerson_Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
		PRINT_LOG(TEXT("My Log : %s"), TEXT("Enemy"));
	}

	// ForCopyFSMComponent = CreateDefaultSubobject<UEnemyFSMComponent>(TEXT("FSMComponent"));
	FSMComponent = CreateDefaultSubobject<UEnemyFSMComponent>(TEXT("FSMComponent"));

	ChargeAttackHitBox = CreateDefaultSubobject<USphereComponent>(TEXT("ChargeAttackHitBox"));
	ChargeAttackHitBox->SetupAttachment(GetCapsuleComponent()/*, FName("RightHandSocket")*/);
	ChargeAttackHitBox->SetSphereRadius(250.f);

	BashAttackHitBox = CreateDefaultSubobject<USphereComponent>(TEXT("BashAttackHitBox"));
	BashAttackHitBox->SetupAttachment(GetCapsuleComponent());
	BashAttackHitBox->SetSphereRadius(250.f);

	// 메쉬 충돌은 끄고 싶다.
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));
	ChargeAttackHitBox->SetCollisionProfileName(TEXT("NoCollision"));
	BashAttackHitBox->SetCollisionProfileName(TEXT("NoCollision"));
}

void AGuardian::BeginPlay()
{
	Super::BeginPlay();

	DefaultColor = GetMesh()->CreateAndSetMaterialInstanceDynamic(0)->K2_GetVectorParameterValue("Paint Tint");
	// FSMComponent = ForCopyFSMComponent;
	InitHP();

	ChargeAttackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AGuardian::OnChargeAttackHitBoxOverlap);
}

void AGuardian::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGuardian::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AGuardian::ExecuteChargeToTarget(AActor* Target, const float ChargeStrength)
{
	// 타겟을 처다보게 하고 싶다.
	FRotator LookAtRotation = (Target->GetActorLocation() - GetActorLocation()).Rotation();
	SetActorRotation(LookAtRotation);

	FVector Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;
	Direction.Normalize();

	FVector LaunchVelocity = Direction * ChargeStrength;
	GetCharacterMovement()->Velocity = LaunchVelocity;
	GetCharacterMovement()->MaxWalkSpeed = ChargeStrength;
	Cast<AAIController>(GetController())->MoveToActor(Target);
}

void AGuardian::SnapToAttackLocation(const AActor* Target, const float AttackRange)
{
	// 정지
	GetCharacterMovement()->StopMovementImmediately();

	// 위치 보정
	FVector Dir = GetActorLocation() - Target->GetActorLocation();
	FVector AttackLocation = Target->GetActorLocation() + Dir.GetSafeNormal() * AttackRange;
	SetActorLocation(AttackLocation);
}

void AGuardian::ExecuteMeleeAttack()
{
	SetActiveChargeAttackHitBox(true);

	FTimerHandle AttackTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		SetActiveChargeAttackHitBox(false);
	}), 0.2f, false);
}

void AGuardian::OnChargeAttackHitBoxOverlap(UPrimitiveComponent* OverlappedComponent,
                                            AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bool bIsDamageApplied = DamageableHelpers::ApplyDamage(OtherActor, this, 20);
	
	SetActiveChargeAttackHitBox(false);
	FSMComponent->FinishAttack(bIsDamageApplied);
}

void AGuardian::SetCharacterMaxWalkSpeed(float InMaxWalkSpeed) const
{
	GetCharacterMovement()->MaxWalkSpeed = InMaxWalkSpeed;
}

void AGuardian::MoveToActor(AActor* Target, float AcceptanceRadius) const
{
	Cast<AAIController>(GetController())->MoveToActor(Target, AcceptanceRadius);
}

void AGuardian::StopMovementImmediately() const
{
	// if (bResetWalkSpeed)
	// {
	// 	GetCharacterMovement()->MaxWalkSpeed = 50.f;
	// }

	// Cast<AAIController>(GetController())->StopMovement();
	GetCharacterMovement()->StopMovementImmediately();
}

void AGuardian::SetActiveChargeAttackHitBox(bool bIsActivate) const
{
	if (bIsActivate)
	{
		ChargeAttackHitBox->SetCollisionProfileName(TEXT("EnemyAttack"));
	}
	else
	{
		ChargeAttackHitBox->SetCollisionProfileName(TEXT("NoCollision"));
	}
}

void AGuardian::SetActiveBashAttackHitBox(bool bIsActivate) const
{
	if (bIsActivate)
	{
		ChargeAttackHitBox->SetCollisionProfileName(TEXT("EnemyAttack"));
	}
	else
	{
		ChargeAttackHitBox->SetCollisionProfileName(TEXT("NoCollision"));
	}
}

bool AGuardian::ReceiveDamage_Implementation(FDamageInfo DamageInfo)
{
	HP -= DamageInfo.DamageAmount;
	if (HP <= 0)
	{
		HP = 0;
		FSMComponent->RequestDead();
	}
	else
	{
		FSMComponent->RequestDamaged(DamageInfo.DamageAmount, HP);;
	}

	return true;
}

int AGuardian::GetHP_Implementation() const
{
	return GetCurrentHP();
}

bool AGuardian::IsDead_Implementation() const
{
	return FSMComponent->IsDead();
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

void AGuardian::InitHP()
{
	HP = MaxHP;
}
