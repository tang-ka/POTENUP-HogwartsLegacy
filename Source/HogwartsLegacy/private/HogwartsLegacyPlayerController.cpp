// Fill out your copyright notice in the Description page of Project Settings.


#include "HogwartsLegacyPlayerController.h"

void AHogwartsLegacyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameAndUI());
}
