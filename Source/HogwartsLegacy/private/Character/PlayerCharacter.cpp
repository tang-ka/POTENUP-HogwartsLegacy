// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Character/BasicAttack.h"
#include "Character/Protego.h"
#include "Character/Stupefy.h"
#include "Components/CapsuleComponent.h"
#include "Enemy/Guardian.h"
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

		PlayerInput->BindAction(IA_Attack, ETriggerEvent::Started, this, &APlayerCharacter::SpellBasicAttack);
		PlayerInput->BindAction(IA_SkillSlot1, ETriggerEvent::Started, this, &APlayerCharacter::SpellSkillSlot1);
		PlayerInput->BindAction(IA_SkillSlot2, ETriggerEvent::Started, this, &APlayerCharacter::SpellSkillSlot2);
		PlayerInput->BindAction(IA_SkillSlot3, ETriggerEvent::Started, this, &APlayerCharacter::SpellSkillSlot3);
		PlayerInput->BindAction(IA_SkillSlotQ, ETriggerEvent::Started, this, &APlayerCharacter::SpellSkillSlotQ);
		PlayerInput->BindAction(IA_SkillSlotQ, ETriggerEvent::Completed, this, &APlayerCharacter::SpellStupefy);
		PlayerInput->BindAction(IA_SkillSlotR, ETriggerEvent::Started, this, &APlayerCharacter::SpellSkillSlotR);
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
	Check = IsValid(atk) || IsValid(prt) || IsValid(stp) ? false : true;
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
			GEngine->AddOnScreenDebugMessage(3, 1.0f, FColor::Green, Obj->GetName());
			AimedTarget = nullptr;
			if (Obj->GetActorNameOrLabel().Contains(TEXT("Guardian")))
			{
				Obj->FindComponentByClass<USkeletalMeshComponent>()->SetOverlayMaterial(OutlineMat);
				AimedTarget = Obj;
			}
			if (Obj->ActorHasTag(FName("Item")))
			{
				Obj->FindComponentByClass<UMeshComponent>()->SetOverlayMaterial(ShineMat);
			}
		}
	}
	LastCachedObj = Obj;
}


void APlayerCharacter::SpellBasicAttack(const FInputActionValue& InputValue)
{
	static int ComboCount = 0;
	static bool ComboAttack;
	if (IsValid(atk))
	{
		if (ComboCount == 2)
		{
			ComboAttack = true;
			ComboCount = 0;
		}
		else
		{
			ComboAttack = false;
			ComboCount++;
		}
	}
	if (CanAttack)
	{
		if (AimedTarget && AimedTarget->GetActorNameOrLabel().Contains(TEXT("Guardian")))
		{
			FVector dir = AimedTarget->GetActorLocation() - this->GetActorLocation();
			dir.Normalize();
			atk = GetWorld()->SpawnActor<ABasicAttack>(BasicAttacks, GetActorLocation(), dir.Rotation());
			atk->SetTarget(this, AimedTarget, ComboAttack);
		}
		else
		{
			atk = GetWorld()->SpawnActor<ABasicAttack>(BasicAttacks, GetActorLocation(), GetActorRotation());
			atk->NoTarget();
		}
	}
}

void APlayerCharacter::SpellSkillSlot1(const FInputActionValue& InputValue)
{
	GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, TEXT("Skill 1"));
}

void APlayerCharacter::SpellSkillSlot2(const FInputActionValue& InputValue)
{
	GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, TEXT("Skill 2"));
}

void APlayerCharacter::SpellSkillSlot3(const FInputActionValue& InputValue)
{
	GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, TEXT("Skill 3"));
}

void APlayerCharacter::SpellSkillSlotQ(const FInputActionValue& InputValue) // Protego
{
	if (CanAttack)
	{
		prt = GetWorld()->SpawnActor<AProtego>(Protegos, GetActorLocation(), GetActorRotation());
		// 타임 스토푸
		// GetWorld()->GetWorldSettings()->SetTimeDilation(0.5f);
	}
}

void APlayerCharacter::SpellStupefy(const FInputActionValue& InputValue)
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

void APlayerCharacter::SpellSkillSlotR(const FInputActionValue& InputValue)
{
	GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, TEXT("Skill R"));
}

bool APlayerCharacter::ReceiveDamage_Implementation(FDamageInfo DamageInfo)
{
	if (prt)
	{
		return false;
	}
	int NewHP = GetCurrentHP() - DamageInfo.DamageAmount;
	UE_LOG(LogTemp, Warning, TEXT("Player received damage: %d, New HP: %d"), DamageInfo.DamageAmount, NewHP);
	if (NewHP <= 0)
	{
		SetCurrentHP(0);
	}
	else
	{
		SetCurrentHP(NewHP);
	}
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
