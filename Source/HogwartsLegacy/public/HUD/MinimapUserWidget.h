// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinimapUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UMinimapUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	APawn* PlayerPawn;
	UPROPERTY(meta = (BindWidget))
	UImage* MinimapTrim;
	UPROPERTY()
	FVector DummyLocation;
	UPROPERTY()
	FVector DummyRotation;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetPlayerRotation();
};
