// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GuardianCombatGameMode.generated.h"

/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API AGuardianCombatGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	virtual void StartPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
