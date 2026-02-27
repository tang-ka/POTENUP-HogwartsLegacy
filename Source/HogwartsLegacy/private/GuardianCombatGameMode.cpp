// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardianCombatGameMode.h"

#include "GuardianManager.h"

void AGuardianCombatGameMode::StartPlay()
{
	Super::StartPlay();
	
	GetWorld()->GetSubsystem<UGuardianManager>()->Enable();
}

