// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Guardian.h"

#include "AIController.h"
#include "GuardianAnim.h"
#include "GuardianShatterComponent.h"
#include "HogwartsLegacy.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Enemy/EnemyFSMComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/DamageableHelpers.h"

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
	}
	
	ShatterComponent = CreateDefaultSubobject<UGuardianShatterComponent>(TEXT("ShatterComponent"));
	FSMComponent = CreateDefaultSubobject<UEnemyFSMComponent>(TEXT("FSMComponent"));
	// FSMComponentForCopy = CreateDefaultSubobject<UEnemyFSMComponent>(TEXT("FSMComponent"));
	
	ChargeAttackHitBox = CreateDefaultSubobject<USphereComponent>(TEXT("ChargeAttackHitBox"));
	ChargeAttackHitBox->SetupAttachment(GetCapsuleComponent()/*, FName("RightHandSocket")*/);
	ChargeAttackHitBox->SetSphereRadius(250.f);

	BashAttackHitBox = CreateDefaultSubobject<USphereComponent>(TEXT("BashAttackHitBox"));
	BashAttackHitBox->SetupAttachment(GetCapsuleComponent());
	BashAttackHitBox->SetSphereRadius(250.f);

	// 메쉬 충돌은 끄고 싶다.
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));

	// AI 컨트롤러 자동 빙의 설정: 스폰된 경우에도 AIController가 할당되도록 함
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	SetActiveChargeAttackHitBox(false);
	SetActiveBashAttackHitBox(false);
	// ChargeAttackHitBox->SetCollisionProfileName(TEXT("NoCollision"));
	// BashAttackHitBox->SetCollisionProfileName(TEXT("NoCollision"));
}

void AGuardian::BeginPlay()
{
	Super::BeginPlay();
	// FSMComponent = FSMComponentForCopy;
	if (!FSMComponent)
	{
		PRINT_LOG(TEXT("FSMComponent is NULL"));
	}
	DefaultColor = GetMesh()->CreateAndSetMaterialInstanceDynamic(0)->K2_GetVectorParameterValue("Paint Tint");
	InitHP();

	ChargeAttackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AGuardian::OnChargeAttackHitBoxOverlap);
	BashAttackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AGuardian::OnBashAttackHitBoxOverlap);
	
	Tags.Add(FName("Enemy"));
}

void AGuardian::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGuardian::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AGuardian::ExecuteChargeToTarget(AActor* InTarget, const float ChargeStrength)
{
	LaunchToTarget(InTarget, ChargeStrength);
}

void AGuardian::SnapToAttackLocation(const AActor* InTarget, const float AttackRange)
{
	// 정지
	GetCharacterMovement()->StopMovementImmediately();

	// 위치 보정
	FVector Dir = GetActorLocation() - InTarget->GetActorLocation();
	FVector AttackLocation = InTarget->GetActorLocation() + Dir.GetSafeNormal() * AttackRange;
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
	// FSMComponent->FinishAttack(false);
}

void AGuardian::ExecuteStumble(float StumbleStrength)
{
	auto* Target = GetFSMComponent()->GetCurrentTarget();
	FVector Direction = GetActorLocation() - Target->GetActorLocation();
	Direction.Z = 0.0f;
	Direction.Normalize();
	
	FVector LaunchVelocity = Direction * StumbleStrength;
	GetCharacterMovement()->BrakingFriction = 0.0f;
	GetCharacterMovement()->Launch(LaunchVelocity);
}

void AGuardian::ExecuteBashAttack(AActor* InTarget, const float BashStrength)
{
	SetActiveBashAttackHitBox(true);
	LaunchToTarget(InTarget, BashStrength);
	
	GetWorld()->GetTimerManager().SetTimer(BashAttackTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		SetActiveBashAttackHitBox(false);
		// 로그: 충돌하지 않고 끝났다 
		PRINT_LOG(TEXT("End Bash Attack without hit"));
	}), 2.0f, false);
}

void AGuardian::OnBashAttackHitBoxOverlap(UPrimitiveComponent* OverlappedComponent,
                                          AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                          int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bool bIsDamageApplied = DamageableHelpers::ApplyDamage(OtherActor, this, 20);
	
	StopMovementImmediately();
	GetWorldTimerManager().ClearTimer(BashAttackTimerHandle);
	SetActiveBashAttackHitBox(false);
	FSMComponent->FinishAttack(bIsDamageApplied); // Bash 공격은 항상 성공으로 간주 (Stumble 상태로 가지 않음)
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

void AGuardian::LaunchToTarget(AActor* InTarget, float Strength, bool bLookAtTarget)
{
	if (bLookAtTarget)
	{
		// 타겟을 처다보게 하고 싶다.
		FRotator LookAtRotation = (InTarget->GetActorLocation() - GetActorLocation()).Rotation();
		SetActorRotation(LookAtRotation);
	}
	
	FVector Direction = InTarget->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;
	Direction.Normalize();

	FVector LaunchVelocity = Direction * Strength;
	GetCharacterMovement()->Velocity = LaunchVelocity;
	GetCharacterMovement()->MaxWalkSpeed = Strength;
	Cast<AAIController>(GetController())->MoveToActor(InTarget);
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
		BashAttackHitBox->SetCollisionProfileName(TEXT("EnemyAttack"));
	}
	else
	{
		BashAttackHitBox->SetCollisionProfileName(TEXT("NoCollision"));
	}
}

bool AGuardian::ReceiveDamage_Implementation(FDamageInfo DamageInfo)
{
	if (FSMComponent->IsDead())
	{
		return false;
	}
	
	int32 DamageAmount = DamageInfo.DamageAmount;
	// DamageAmount = 50;
	
	HP -= DamageAmount;
	if (HP <= 0)
	{
		HP = 0;
		FSMComponent->RequestDead();
	}
	else
	{
		if (DamageAmount < StrongAttackThreshold)
		{
			if (auto* Anim = Cast<UGuardianAnim>(GetMesh()->GetAnimInstance()))
			{
				Anim->PlayBasicDamagedMontage();
			}
		}
		else
		{
			FSMComponent->RequestDamaged(DamageAmount, HP);;
		}
	}

	return true;
}

int AGuardian::GetHP_Implementation() const
{
	return GetCurrentHP();
}

float AGuardian::GetHPPercent_Implementation() const
{
	return static_cast<float>(GetCurrentHP()) / static_cast<float>(MaxHP);
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

// ============================================================
// Combat Info — Manager에게 판단 정보 제공
// ============================================================

bool AGuardian::IsAttackReady() const
{
	if (!FSMComponent)
	{
		return false;
	}
	
	const EEnemyState State = FSMComponent->GetCurrentStateType();
	return State == EEnemyState::Strafe;
}

float AGuardian::GetDistanceToTarget() const
{
	const AActor* Target = FSMComponent ? FSMComponent->GetCurrentTarget() : nullptr;
	if (!Target)
	{
		return TNumericLimits<float>::Max();
	}
	
	return FVector::Dist(GetActorLocation(), Target->GetActorLocation());
}

float AGuardian::GetAngleToTarget() const
{
	const AActor* Target = FSMComponent ? FSMComponent->GetCurrentTarget() : nullptr;
	if (!Target)
	{
		return 180.f;
	}
	
	const FVector Forward = GetActorForwardVector();
	FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
	ToTarget.Z = 0.f;
	ToTarget.Normalize();
	
	const float Dot = FVector::DotProduct(Forward, ToTarget);
	return FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.f, 1.f)));
}

FVector2D AGuardian::GetAnimWalkDirection()
{
	FVector Velocity = GetVelocity();
	
	if (Velocity.IsNearlyZero())
	{
		WalkDirection = FVector2D::ZeroVector;
		return WalkDirection;
	}
	
	
	FVector LocalVelocity = GetActorRotation().UnrotateVector(Velocity);
	LocalVelocity.Z = 0.f;
	LocalVelocity.Normalize();
	
	WalkDirection.Y = LocalVelocity.X;
	WalkDirection.X = LocalVelocity.Y;
	
	return WalkDirection;
}

