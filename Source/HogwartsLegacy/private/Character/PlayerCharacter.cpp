

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
	// static ConstructorHelpers::FObjectFinder<USkeletalMesh> WandMesh(TEXT("/Game/KHJ/Meshes/PlayerWand.PlayerWand"));
	// if (WandMesh.Succeeded())
	// {
	// 	Wand->SetSkeletalMeshAsset(WandMesh.Object);
	// }
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
	WandTipComp->SetRelativeLocation(GetMesh()->GetBoneLocation(FName(TEXT("WandSocketTip"))));

	// Levioso
	PhysicsHandleComp = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	PhysicsHandleComp->LinearDamping = 200.0f; // 높을수록 덜 출렁임
	PhysicsHandleComp->LinearStiffness = 1500.0f; // 잡는 힘의 강도
	PhysicsHandleComp->InterpolationSpeed = 10.0f; // 추적 속도 (천천히 따라오게 조절)
	
	bUseControllerRotationYaw = true;
	
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC)
	{
		PC->SetPause(false);
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
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
	
	// Wand->AttachToComponent(WandTipComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("WandSocketTip")));
	AnimInstance = GetMesh()->GetAnimInstance();
	HUD = Cast<AControllerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Level"), ActorToIgnore);
	AimCaptureParams.AddIgnoredActors(ActorToIgnore);
	AimCaptureParams.AddIgnoredActor(this);
}

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
	if (bCanMove)
	{
		Jump();
	}
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
	FHitResult HitOut;
	
	FVector Start = PlayerCameraComp->GetComponentLocation();
	FVector End = PlayerCameraComp->GetComponentRotation().Vector() * ViewDistance;
	
	GetWorld()->SweepSingleByProfile(HitOut,
		Start, End, FQuat::Identity, "PlayerTrace",
		FCollisionShape::MakeSphere(ViewSphereRadius), AimCaptureParams);
	
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
	SpellProtego();
}

void APlayerCharacter::SkillSlotQLongClick(const FInputActionValue& InputValue)
{
	SpellStupefy();
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
	if (GetWorldTimerManager().IsTimerActive(ComboTimerHandle))
	{
		ComboCount++;
		// GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Blue, FString::Printf(TEXT("ComboCount: %d"), ComboCount));
	}
	else
	{
		ComboCount = 0;
	}

	if (ComboCount >= 4)
	{
		bComboAttack = true;
	}

	if (!AnimInstance->Montage_IsPlaying(ComboAttackMontage))
	{
		bCanMove = false;
		PlayAnimMontage(ComboAttackMontage, 1.0f, FName(FString::Printf(TEXT("ComboCount%d"), ComboCount)));
	}
	else
	{
		bCanMove = false;
		AnimInstance->Montage_JumpToSection(FName(FString::Printf(TEXT("ComboCount%d"), ComboCount)), ComboAttackMontage);
	}
	
	
	GetWorldTimerManager().SetTimer(ComboTimerHandle,[this](){this->ComboCount=0;}, 2.0f, false);
}

void APlayerCharacter::SpellProtego()
{
	if (bCanAttack)
	{
		if (!AnimInstance->Montage_IsPlaying(ProtegoMontage))
		{
			bCanMove = false;
			PlayAnimMontage(ProtegoMontage);
		}
		// GetWorld()->GetWorldSettings()->SetTimeDilation(0.5f);
	}
}


void APlayerCharacter::SpellLumos(USceneComponent* WandTip)
{
	if (!IsValid(lumos))
	{
		HUD->UpdateSkillSlotWidget(1, TEXT("Lumos"));
		lumos = GetWorld()->SpawnActor<ALumos>(Lumoses, GetMesh()->GetBoneLocation(FName(TEXT("WandSocketTip"))), WandTipComp->GetComponentRotation());
		lumos->SetLocation(GetMesh(), GetMesh()->GetSocketByName(FName(TEXT("WandTipSocket"))));
	}
	else
	{
		lumos->Destroy();
	}
}

void APlayerCharacter::SpellStupefy()
{
	if (bCanAttack)
	{
		bCanMove = false;
		PlayAnimMontage(StupefyMontage);
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
		attack = GetWorld()->SpawnActor<ABasicAttack>(BasicAttacks, GetMesh()->GetBoneLocation(FName(TEXT("WandSocketTip"))), dir.Rotation());
		attack->SetTarget(this, AimedTarget, bComboAttack);
	}
	else
	{
		attack = GetWorld()->SpawnActor<ABasicAttack>(BasicAttacks, GetMesh()->GetBoneLocation(FName(TEXT("WandSocketTip"))), GetViewRotation());
		attack->NoTarget();
	}
	if (bComboAttack)
	{
		ComboCount = 0;
		bComboAttack = false;
	}
}

void APlayerCharacter::SpawnProtego()
{
	protego = GetWorld()->SpawnActor<AProtego>(Protegos, GetActorLocation(), GetActorRotation());
}

void APlayerCharacter::SpawnStupefy()
{
	if (AimedTarget && AimedTarget->ActorHasTag(FName("Enemy")))
	{
		FVector dir = AimedTarget->GetActorLocation() - this->GetActorLocation();
		dir.Normalize();
		stupefy = GetWorld()->SpawnActor<AStupefy>(Stupefies, GetMesh()->GetBoneLocation(FName(TEXT("WandSocketTip"))), dir.Rotation());
		stupefy->SetTarget(AimedTarget);
	}
	else
	{
		stupefy = GetWorld()->SpawnActor<AStupefy>(Stupefies, GetMesh()->GetBoneLocation(FName(TEXT("WandSocketTip"))), GetActorRotation());
		stupefy->NoTarget();
	}
}

bool APlayerCharacter::ReceiveDamage_Implementation(FDamageInfo DamageInfo)
{
	if (IsValid(protego))
	{
		return false;
	}
	int NewHP = GetCurrentHP() - DamageInfo.DamageAmount;
	if (NewHP <= 0)
	{
		NewHP = 0;
		HUD->Gameover();
	}
	SetCurrentHP(NewHP);
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
