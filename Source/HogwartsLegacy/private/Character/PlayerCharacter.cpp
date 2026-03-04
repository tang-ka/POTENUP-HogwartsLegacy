

#include "Character/PlayerCharacter.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Character/BasicAttack.h"
#include "Character/Lumos.h"
#include "Character/Protego.h"
#include "Character/Stupefy.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("SkeletalMesh'/Game/KHJ/Characters/SM_PlayerCharacter.SM_PlayerCharacter'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}
	// static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstance (TEXT("AnimBlueprint'/Game/KHJ/Characters/Animation/ABP_PlayerCharacter.ABP_PlayerCharacter'"));
	// if (AnimInstance.Succeeded())
	// {
	// 	GetMesh()->SetAnimInstanceClass(AnimInstance.Class);
	// }

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("PlayerCharacter"));
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	CameraArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArmComp->SetupAttachment(RootComponent);
	CameraArmComp->SetRelativeLocation(FVector(0, 125, 60));
	CameraArmComp->TargetArmLength = 250;
	CameraArmComp->bUsePawnControlRotation = true;
	
	PlayerCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCameraComp->SetupAttachment(CameraArmComp);
	PlayerCameraComp->bUsePawnControlRotation = false;
	
	WandTipComp = CreateDefaultSubobject<USceneComponent>(TEXT("WandTip"));
	WandTipComp->SetupAttachment(RootComponent);
	WandTipComp->SetRelativeLocation(GetMesh()->GetSocketLocation(FName(TEXT("WandSocketTipSocket"))));
	
	// Levioso
	PhysicsHandleComp = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	PhysicsHandleComp->LinearDamping = 200.0f; // 높을수록 덜 출렁임
	PhysicsHandleComp->LinearStiffness = 1500.0f; // 잡는 힘의 강도
	PhysicsHandleComp->InterpolationSpeed = 10.0f; // 추적 속도 (천천히 따라오게 조절)
	
	bUseControllerRotationYaw = true;
	
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(IMC_Player, 0);
		}
	}
	
	if (IA_SkillSlotQ) // Change the Hold Time Threshold to 2.0 sec
	{
		bool bFoundTrigger = false;
		for (UInputTrigger* Trigger : IA_SkillSlotQ->Triggers)
		{
			if (UInputTriggerHold* HoldTrigger = Cast<UInputTriggerHold>(Trigger))
			{
				HoldTrigger->HoldTimeThreshold = 2.0f;
				bFoundTrigger = true;
				break;
			}
		}

		if (!bFoundTrigger)
		{
			UInputTriggerHold* NewHoldTrigger = NewObject<UInputTriggerHold>(IA_SkillSlotQ);
			NewHoldTrigger->HoldTimeThreshold = 2.0f;
			IA_SkillSlotQ->Triggers.Add(NewHoldTrigger);
		}
	}
	
	AnimInstance = GetMesh()->GetAnimInstance();
	HUD = Cast<AControllerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Level"), ActorToIgnore);
	AimCaptureParams.AddIgnoredActors(ActorToIgnore);
	AimCaptureParams.AddIgnoredActor(this);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bCanMove)
	{
		PlayerMove();
	}
	CheckCanAttack(bCanAttack);
	AimCapture();
	
	if (bIsDoingLevioso && PhysicsHandleComp->GrabbedComponent)
	{
		// 1. 기본 목표 위치 계산 (카메라 앞)
		FVector TargetLocation = GetPawnViewLocation() + (GetViewRotation().Vector() * LeviosoHoldDistance);
		
		LeviosoFloatTimer += DeltaTime * LeviosoFloatSpeed;
		float ZOffset = FMath::Sin(LeviosoFloatTimer) * LeviosoFloatAmplitude;
		TargetLocation.Z += ZOffset;
		
		PhysicsHandleComp->SetTargetLocationAndRotation(TargetLocation, GetViewRotation());
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput)
	{
		PlayerInput->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &APlayerCharacter::Turn);
		PlayerInput->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &APlayerCharacter::LookUp);
		PlayerInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		PlayerInput->BindAction(IA_Run, ETriggerEvent::Started, this, &APlayerCharacter::Run);
		PlayerInput->BindAction(IA_Jump, ETriggerEvent::Started, this, &APlayerCharacter::InputJump);

		PlayerInput->BindAction(IA_Attack, ETriggerEvent::Started, this, &APlayerCharacter::SkillMouseL);
		PlayerInput->BindAction(IA_SkillSlot1, ETriggerEvent::Started, this, &APlayerCharacter::SkillSlot1);
		PlayerInput->BindAction(IA_SkillSlot2, ETriggerEvent::Started, this, &APlayerCharacter::SkillSlot2);
		PlayerInput->BindAction(IA_SkillSlot3, ETriggerEvent::Started, this, &APlayerCharacter::SkillSlot3);
		PlayerInput->BindAction(IA_SkillSlotQ, ETriggerEvent::Started, this, &APlayerCharacter::SkillSlotQ);
		PlayerInput->BindAction(IA_SkillSlotQ, ETriggerEvent::Completed, this, &APlayerCharacter::SkillSlotQLongClick);
		PlayerInput->BindAction(IA_SkillSlotR, ETriggerEvent::Started, this, &APlayerCharacter::SkillSlotR);
	}
}

void APlayerCharacter::Turn(const FInputActionValue& InputValue)
{
	AddControllerYawInput(InputValue.Get<float>());
}

void APlayerCharacter::LookUp(const FInputActionValue& InputValue)
{
	AddControllerPitchInput(InputValue.Get<float>());
}

void APlayerCharacter::Move(const FInputActionValue& InputValue)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	WalkDirection.X = InputValue.Get<FVector2D>().X;
	WalkDirection.Y = InputValue.Get<FVector2D>().Y;
}

void APlayerCharacter::Run(const FInputActionValue& InputValue)
{
	static bool bRun = false;
	bRun = !bRun;
	WalkSpeed = bRun ? 500 : 250;
}

void APlayerCharacter::InputJump(const FInputActionValue& InputValue)
{
	Jump();
}

void APlayerCharacter::PlayerMove()
{
	WalkDirection = FTransform(GetControlRotation()).TransformVector(WalkDirection);
	AddMovementInput(WalkDirection);
	WalkDirection = FVector::ZeroVector;
}

void APlayerCharacter::CheckCanAttack(bool& Check)
{
	Check = IsValid(attack) || IsValid(protego) || IsValid(stupefy) ? false : true;
}

void APlayerCharacter::AimCapture()
{
	FVector Start;
	FRotator Rot;
	FHitResult HitOut;
	
	GetController()->GetPlayerViewPoint(Start, Rot);
	FVector End = Rot.Vector() * ViewDistance;
	
	GetWorld()->SweepSingleByProfile(HitOut, Start, End, FQuat::Identity, "PlayerTrace", FCollisionShape::MakeSphere(ViewSphereRadius), AimCaptureParams);
	
	TObjectPtr<AActor> Obj = HitOut.GetActor();
	HUD->OverlayMatOnAimCaptured(Obj, LastCachedObj, AimedTarget);
	LastCachedObj = Obj;
}


void APlayerCharacter::SkillMouseL(const FInputActionValue& InputValue)
{
	SpellBasicAttack();
}

void APlayerCharacter::SkillSlot1(const FInputActionValue& InputValue)
{
	SpellLumos(WandTipComp);
}

void APlayerCharacter::SkillSlot2(const FInputActionValue& InputValue)
{
	GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, TEXT("Skill 2"));
}

void APlayerCharacter::SkillSlot3(const FInputActionValue& InputValue)
{
	GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, TEXT("Skill 3"));
}

void APlayerCharacter::SkillSlotQ(const FInputActionValue& InputValue)
{
	SpellProtego(protego);
}

void APlayerCharacter::SkillSlotQLongClick(const FInputActionValue& InputValue)
{
	SpellStupefy(stupefy);
}


void APlayerCharacter::SkillSlotR(const FInputActionValue& InputValue)
{
	GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, TEXT("Skill R"));
}




void APlayerCharacter::SpellBasicAttack()
{
	if (!bCanAttack)
	{
		return;
	}
	else
	{
		if (GetWorldTimerManager().IsTimerActive(ComboTimerHandle)) // Check Combo Timing
		{
			ComboCount++;
		}
		else
		{
			ComboCount = 0;
		}
	
		if (ComboCount >= 4) // Check Whether Is This ComboAttack
		{
			ComboCount = 0;
			bComboAttack = true;
		}
		
		if (!AnimInstance->Montage_IsPlaying(ComboAttackMontage))
		{
			bCanMove = false;
			PlayAnimMontage(ComboAttackMontage, 1.0f, FName(*FString::Printf(TEXT("ComboAttack%d"), ComboCount)));
			// switch (ComboCount)
			// {
			// case 2: AnimInstance->Montage_JumpToSection(FName(TEXT("ComboAttack2")));
			// case 3: AnimInstance->Montage_JumpToSection(FName(TEXT("ComboAttack3")));
			// case 4: AnimInstance->Montage_JumpToSection(FName(TEXT("ComboAttack4")));
			// default: AnimInstance->Montage_Play(ComboAttackMontage);
			// }
		}
		else
		{
			// 이미 재생 중이면 해당 섹션으로 바로 점프
			AnimInstance->Montage_JumpToSection(FName(*FString::Printf(TEXT("ComboAttack%d"), ComboCount)), ComboAttackMontage);
		}
		

	}
	GetWorldTimerManager().SetTimer(ComboTimerHandle,[this](){this->ComboCount=0;}, 1.0f, false);
	bComboAttack = false;
}

void APlayerCharacter::SpellProtego(AProtego* prt)
{
	if (bCanAttack)
	{
		prt = GetWorld()->SpawnActor<AProtego>(Protegos, GetActorLocation(), GetActorRotation());
		// GetWorld()->GetWorldSettings()->SetTimeDilation(0.5f);
	}
}


void APlayerCharacter::SpellLumos(USceneComponent* WandTip)
{
	if (!IsValid(lumos))
	{
		lumos = GetWorld()->SpawnActor<ALumos>(Lumoses, WandTip->GetComponentLocation(), WandTipComp->GetComponentRotation());
		lumos->SetLocation(WandTip);
	}
	else
	{
		lumos->Destroy();
	}
}

void APlayerCharacter::SpellStupefy(AStupefy* stp)
{
	if (bCanAttack)
	{
		if (AimedTarget && AimedTarget->ActorHasTag(FName("Enemy")))
		{
			FVector dir = AimedTarget->GetActorLocation() - this->GetActorLocation();
			dir.Normalize();
			stp = GetWorld()->SpawnActor<AStupefy>(Stupefies, WandTipComp->GetComponentLocation(), dir.Rotation());
			stp->SetTarget(AimedTarget);
		}
		else
		{
			stp = GetWorld()->SpawnActor<AStupefy>(Stupefies, WandTipComp->GetComponentLocation(), GetActorRotation());
			stp->NoTarget();
		}
	}
	
}

void APlayerCharacter::SpellLevioso(UPrimitiveComponent* TargetComp, FVector Location, FRotator Rotation)
{
	if (!bCanAttack)
	{
		return;
	}
	if (TargetComp && TargetComp->IsSimulatingPhysics())
	{
		if (bIsDoingLevioso && PhysicsHandleComp->GrabbedComponent)
		{
			PhysicsHandleComp->ReleaseComponent();
			bIsDoingLevioso = false;
			LeviosoFloatTimer = 0.f;
		}
		PhysicsHandleComp->GrabComponentAtLocationWithRotation(TargetComp, NAME_None, Location, Rotation);
		bIsDoingLevioso = true;
	}
}

void APlayerCharacter::SpellDescendo()
{
}

void APlayerCharacter::SpawnBasicAttack()
{
	if (AimedTarget && AimedTarget->ActorHasTag(FName("Enemy")))
	{
		FVector dir = AimedTarget->GetActorLocation() - this->GetActorLocation();
		dir.Normalize();
		attack = GetWorld()->SpawnActor<ABasicAttack>(BasicAttacks, WandTipComp->GetComponentLocation(), dir.Rotation());
		attack->SetTarget(this, AimedTarget, bComboAttack);
	}
	else
	{
		attack = GetWorld()->SpawnActor<ABasicAttack>(BasicAttacks, WandTipComp->GetComponentLocation(), GetViewRotation());
		attack->NoTarget();
		ReceiveDamage_Implementation(FDamageInfo{ this, 20});
	}
}

bool APlayerCharacter::ReceiveDamage_Implementation(FDamageInfo DamageInfo)
{
	if (protego)
	{
		return false;
	}
	int NewHP = GetCurrentHP() - DamageInfo.DamageAmount;
	SetCurrentHP(NewHP = NewHP <= 0 ? 0 : NewHP);
	HUD->UpdatePlayerHPWidget(GetHPPercent_Implementation());
	return true;
}

int APlayerCharacter::GetHP_Implementation() const
{
	return GetCurrentHP();
}

float APlayerCharacter::GetHPPercent_Implementation() const
{
	return static_cast<float>(GetCurrentHP()) / static_cast<float>(GetDefaultHP());
}

bool APlayerCharacter::IsDead_Implementation() const
{
	return GetCurrentHP() <= 0;
}
