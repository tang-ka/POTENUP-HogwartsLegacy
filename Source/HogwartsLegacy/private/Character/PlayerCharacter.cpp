

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
	
	MagicRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagicRing"));
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("SkeletalMesh'/Game/KHJ/Characters/SM_PlayerCharacter.SM_PlayerCharacter'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh2(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (TempMesh2.Succeeded())
	{
		MagicRing->SetStaticMesh(TempMesh2.Object);
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterial> TempMat(TEXT("/Game/KHJ/FX/Material/T_bottom13_1.T_bottom13_1"));
	if (TempMat.Succeeded())
	{
		MagicRing->SetMaterial(0, TempMat.Object);
	}

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("PlayerCharacter"));
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
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
	
	MagicRing->SetupAttachment(RootComponent);
	
	bUseControllerRotationYaw = true;
	
}

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
	
	if (IA_SkillSlotQ) // Change the Hold Time Threshold to 1.5 sec
	{
		bool bFoundTrigger = false;
		for (UInputTrigger* Trigger : IA_SkillSlotQ->Triggers)
		{
			if (UInputTriggerHold* HoldTrigger = Cast<UInputTriggerHold>(Trigger))
			{
				HoldTrigger->HoldTimeThreshold = 1.5f;
				bFoundTrigger = true;
				break;
			}
		}

		if (!bFoundTrigger)
		{
			UInputTriggerHold* NewHoldTrigger = NewObject<UInputTriggerHold>(IA_SkillSlotQ);
			NewHoldTrigger->HoldTimeThreshold = 1.5f;
			IA_SkillSlotQ->Triggers.Add(NewHoldTrigger);
		}
	}
	
	AnimInstance = GetMesh()->GetAnimInstance();
	HUD = Cast<AControllerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Level"), ActorToIgnore);
	AimCaptureParams.AddIgnoredActors(ActorToIgnore);
	AimCaptureParams.AddIgnoredActor(this);
	MagicRing->SetVisibility(false);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bCanMove)
	{
	 	PlayerMove();
	}
	CheckCanAttack(bCanAttack);
	CheckCanMove(bCanMove);
	AimCapture();
	
}

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
	WalkSpeed = bRun ? 600 : 300;
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
	if (AnimInstance)
	{
		AnimInstance->StopAllMontages(0.5f);
	}
	AddMovementInput(WalkDirection);
	WalkDirection = FVector::ZeroVector;
}

void APlayerCharacter::CheckCanAttack(bool& Check)
{
	Check = IsValid(attack) || IsValid(protego) || IsValid(stupefy) ? false : true;
}

void APlayerCharacter::CheckCanMove(bool& Check)
{
	if (AnimInstance)
	{
		if (!AnimInstance->IsAnyMontagePlaying())
		{
			Check = true;
		}
	}
}

void APlayerCharacter::AimCapture()
{
	FHitResult HitOut;
	FVector Start;
	FRotator Rot;
	
	GetController()->GetPlayerViewPoint(Start, Rot);
	FVector End = Rot.Vector() * ViewDistance;
	
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
	bGodMode = !bGodMode;
	GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red, FString::Printf(TEXT("GodMode: %s"), bGodMode ? TEXT("ON") : TEXT("OFF")));
}

void APlayerCharacter::SkillSlot3(const FInputActionValue& InputValue)
{
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
}

void APlayerCharacter::AboutToBeAttacked(bool bActive)
{
	MagicRing->SetVisibility(bActive);
}

void APlayerCharacter::SpellBasicAttack()
{
	if (!bCanAttack)
	{
		return;
	}
	
	if (bCanComboInput)
	{
		ComboCount++;
	}
	else
	{
		ComboCount = 0;
	}
	
	if (ComboCount >= 4)
	{
		bComboAttack = true;
		ComboCount = 0;
	}
	
	bCanComboInput = true;
	bCanMove = false;
	
	if (!AnimInstance->Montage_IsPlaying(ComboAttackMontage))
	{
		PlayAnimMontage(ComboAttackMontage, 1.0f, FName(FString::Printf(TEXT("ComboCount%d"), ComboCount)));
	}
	else
	{
		AnimInstance->Montage_JumpToSection(FName(FString::Printf(TEXT("ComboCount%d"), ComboCount)), ComboAttackMontage);
	}
}

void APlayerCharacter::CheckCombo()
{
	bCanComboInput = false;
	bCanMove = true;
}

void APlayerCharacter::SpellProtego()
{
	if (bCanAttack)
	{
		if (!AnimInstance->Montage_IsPlaying(ProtegoMontage))
		{
			if (MagicRing->IsVisible()) 
			{
				GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(DefenseCameraShakeClass);
				MagicRing->SetVisibility(false);
			}
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

void APlayerCharacter::SpawnBasicAttack()
{
	if (AimedTarget && AimedTarget->ActorHasTag(FName("Enemy")))
	{
		FVector dir = AimedTarget->GetActorLocation() - this->GetActorLocation();
		dir.Normalize();
		attack = GetWorld()->SpawnActor<ABasicAttack>(BasicAttacks, GetMesh()->GetBoneLocation(FName(TEXT("WandSocketTip"))), dir.Rotation());
		if (!IsValid(attack))
		{
			return;
		}
		attack->SetTarget(this, AimedTarget, bComboAttack);
	}
	else
	{
		attack = GetWorld()->SpawnActor<ABasicAttack>(BasicAttacks, GetMesh()->GetBoneLocation(FName(TEXT("WandSocketTip"))), GetViewRotation());
		if (!IsValid(attack))
		{
			return;
		}
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
	if (bGodMode)  // Debug: skip HP reduction
	{
		return false;
	}
	int NewHP = GetCurrentHP() - DamageInfo.DamageAmount;
	bCanMove = false;
	if (NewHP <= 0)
	{
		NewHP = 0;
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		PlayAnimMontage(HitMontage);
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
