// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinimapUserWidget.generated.h"

class UCanvasPanel;
class UImage;
/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UMinimapUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY()
	APlayerController* PlayerController;
	UPROPERTY()
	APawn* PlayerPawn;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MainCanvas;
	UPROPERTY(meta = (BindWidget))
	UImage* MinimapTrim;
	UPROPERTY(meta = (BindWidget))
	UImage* PlayerArrow;
	UPROPERTY(meta = (BindWidget))
	UImage* EnemyArrow;

	UFUNCTION()
	void FindEnemy();
	
	UPROPERTY()
	FVector DummyLocation;
	UPROPERTY()
	FVector DummyRotation;
	
	
	
};
