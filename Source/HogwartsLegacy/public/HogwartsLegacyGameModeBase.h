// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "HogwartsLegacyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API AHogwartsLegacyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AHogwartsLegacyGameModeBase();

	UFUNCTION(BlueprintCallable)
	void SetMiniMapWidget(TSubclassOf<UUserWidget> NewWidget);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf <UUserWidget> Widgets;
	UPROPERTY()
	UUserWidget* MinimapWidget;
};
