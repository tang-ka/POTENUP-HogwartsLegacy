// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardianCombatGameMode.h"

#include "Enemy/GuardianManager.h"

void AGuardianCombatGameMode::StartPlay()
{
	GetWorld()->GetSubsystem<UGuardianManager>()->Enable();
	PRINT_LOG(TEXT("Guardian Combat Game Mode Started. Guardian Manager Enabled."));
	
	Super::StartPlay();
}

void AGuardianCombatGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	GetWorld()->GetSubsystem<UGuardianManager>()->Disable();
	PRINT_LOG(TEXT("Guardian Combat Game Mode Ended. Guardian Manager Disabled."));
}

