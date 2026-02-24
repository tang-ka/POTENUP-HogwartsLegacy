// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"
#include "Character/BasicAttack.h"
#include "Character/Protego.h"
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
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	PlayerMove();
	
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

void APlayerCharacter::SpellBasicAttack(const FInputActionValue& InputValue)
{
	if (!IsValid(atk))
	{
		if (AActor* target = UGameplayStatics::GetActorOfClass(GetWorld(), AGuardian::StaticClass()))
		{
			FVector dir = target->GetActorLocation() - this->GetActorLocation();
			dir.Normalize();
			if (BasicAttackComboCounter == 2) // is third attack?
			{
				GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, TEXT("ThirdAttack"));
				BasicAttackComboCounter = 0;
			} else
			{
				BasicAttackComboCounter++;
			}
			atk = GetWorld()->SpawnActor<ABasicAttack>(BasicAttacks, GetActorLocation(), dir.Rotation());
			atk->SetTarget(target);
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
	if (!IsValid(prt))
	{
		prt = GetWorld()->SpawnActor<AProtego>(Protegos, GetActorLocation(), GetActorRotation());
		// 타임 스토푸
		// GetWorld()->GetWorldSettings()->SetTimeDilation(0.5f);
	}
}

void APlayerCharacter::SpellSkillSlotR(const FInputActionValue& InputValue)
{
	GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, TEXT("Skill R"));
}
