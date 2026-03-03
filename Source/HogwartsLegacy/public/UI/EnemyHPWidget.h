// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "EnemyHPWidget.generated.h"

class UCanvasPanel;
/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UEnemyHPWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UCanvasPanel* HPBarPanel;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UProgressBar* HPBar;

public:
	void UpdateHPBar(float NewPercent);
};
