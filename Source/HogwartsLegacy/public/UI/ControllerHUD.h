// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ControllerHUD.generated.h"

/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API AControllerHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	AControllerHUD();
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMinimapWidget> MinimapWidgetClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UEnemyHPWidget> EnemyHPWidgetClass;
	UPROPERTY()
	UMinimapWidget* MinimapWidget;
	UPROPERTY()
	UEnemyHPWidget* EnemyHPWidget;
	
public:
	UFUNCTION(BlueprintCallable)
	void SetMiniMapWidget(TSubclassOf<UUserWidget> NewWidget);
	UFUNCTION(BlueprintCallable)
	void SetEnemyHPWidget(TSubclassOf<UUserWidget> NewWidget);
};
