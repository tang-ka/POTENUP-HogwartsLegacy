// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Character/BasicAttack.h"
#include "Character/Lumos.h"
#include "Character/Protego.h"
#include "Character/Stupefy.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("SkeletalMesh'/Game/KHJ/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}
	
	ConstructorHelpers::FObjectFinder<UMaterial> TempMat(TEXT("M_Outline'/Game/KHJ/Materials/M_Outline.M_Outline'"));
	if (TempMat.Succeeded())
	{
		OutlineMat = TempMat.Object;
	}
	ConstructorHelpers::FObjectFinder<UMaterial> TempMat2(TEXT("M_ShineEffect'/Game/KHJ/Materials/M_ShineEffect.M_ShineEffect'"));
	if (TempMat2.Succeeded())
	{
		ShineMat = TempMat2.Object;
	}

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
	WandTipComp->SetRelativeLocation(FVector(0, 60, 88));
	
	bUseControllerRotationYaw = true;
	
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PC = Cast<APlayerController>(Controller);
	// auto* PC = GetWorld()->GetFirstPlayerController();
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
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	PlayerMove();
	CheckCanAttack(CanAttack);
	AimCapture();
	
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
	WalkSpeed = bRun ? 500 : 300;
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
	FCollisionQueryParams Params;
	
	GetController()->GetPlayerViewPoint(Start, Rot);
	FVector End = Rot.Vector() * ViewDistance;
	
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Level"), ActorToIgnore);
	Params.AddIgnoredActors(ActorToIgnore);
	Params.AddIgnoredActor(this);
	
	GetWorld()->SweepSingleByProfile(HitOut, Start, End, FQuat::Identity, "PlayerTrace", FCollisionShape::MakeSphere(ViewSphereRadius), Params);
	
	AActor* Obj = HitOut.GetActor();
	
	if (Obj != LastCachedObj)
	{
		if (IsValid(LastCachedObj))
		{
			AimedTarget = nullptr;
			if (LastCachedObj->GetActorNameOrLabel().Contains(TEXT("Guardian")))
			{
				LastCachedObj->FindComponentByClass<USkeletalMeshComponent>()->SetOverlayMaterial(nullptr);
			}
			if (LastCachedObj->ActorHasTag(FName("Item")))
			{
				LastCachedObj->FindComponentByClass<UMeshComponent>()->SetOverlayMaterial(nullptr);
			}
		}
		if (IsValid(Obj))
		{
			// GEngine->AddOnScreenDebugMessage(3, 1.0f, FColor::Green, Obj->GetName());
			AimedTarget = nullptr;
			if (Obj->GetActorNameOrLabel().Contains(TEXT("Guardian")))
			{
				Obj->FindComponentByClass<USkeletalMeshComponent>()->SetOverlayMaterial(OutlineMat);
				AimedTarget = Obj;
			}
			if (Obj->ActorHasTag(FName("Item")) && IsValid(lumos))
			{
				Obj->FindComponentByClass<UMeshComponent>()->SetOverlayMaterial(ShineMat);
			}
		}
	}
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
	if (!CanAttack)
	{
		return;
	}
	else
	{
		if (GetWorldTimerManager().IsTimerActive(ComboTimerHandle)) // Check Combo Timing
		{
			ComboCount++;
			GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Blue, FString::Printf(TEXT("ComboCount: %d"), ComboCount));
		}
		else
		{
			ComboCount = 0;
		}
	
		if (ComboCount >= 4) // Check Whether Is This ComboAttack
		{
			ComboCount = 0;
			ComboAttack = true;
		}
		if (AimedTarget && AimedTarget->GetActorNameOrLabel().Contains(TEXT("Guardian")))
		{
			FVector dir = AimedTarget->GetActorLocation() - this->GetActorLocation();
			dir.Normalize();
			attack = GetWorld()->SpawnActor<ABasicAttack>(BasicAttacks, GetActorLocation(), dir.Rotation());
			attack->SetTarget(this, AimedTarget, ComboAttack);
		}
		else
		{
			attack = GetWorld()->SpawnActor<ABasicAttack>(BasicAttacks, GetActorLocation(), GetViewRotation());
			attack->NoTarget();
		}
	}
	GetWorldTimerManager().SetTimer(ComboTimerHandle,
		[this](){this->ComboCount=0;}, 1.0f, false);
	ComboAttack = false;
}

void APlayerCharacter::SpellProtego(AProtego* prt)
{
	if (CanAttack)
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
	if (CanAttack)
	{
		if (AimedTarget && AimedTarget->GetActorNameOrLabel().Contains(TEXT("Guardian")))
		{
			FVector dir = AimedTarget->GetActorLocation() - this->GetActorLocation();
			dir.Normalize();
			stp = GetWorld()->SpawnActor<AStupefy>(Stupefies, GetActorLocation(), dir.Rotation());
			stp->SetTarget(AimedTarget);
		}
		else
		{
			stp = GetWorld()->SpawnActor<AStupefy>(Stupefies, GetActorLocation(), GetActorRotation());
			stp->NoTarget();
		}
	}
	
}

void APlayerCharacter::SpellLevioso()
{
	if (!CanAttack)
	{
		return;
	}
	else
	{
		
	}
}

void APlayerCharacter::SpellDescendo()
{
}

bool APlayerCharacter::ReceiveDamage_Implementation(FDamageInfo DamageInfo)
{
	if (protego)
	{
		return false;
	}
	int NewHP = GetCurrentHP() - DamageInfo.DamageAmount;
	SetCurrentHP(NewHP = NewHP <= 0 ? 0 : NewHP);
	return true;
}

int APlayerCharacter::GetHP_Implementation() const
{
	return GetCurrentHP();
}

bool APlayerCharacter::IsDead_Implementation() const
{
	return GetCurrentHP() <= 0;
}
