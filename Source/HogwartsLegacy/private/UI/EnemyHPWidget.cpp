// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EnemyHPWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UEnemyHPWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!FMath::IsNearlyEqual(CurrentHPPercent, InterpTargetHPPercent, 0.001f))
	{
		CurrentHPPercent = FMath::FInterpTo(CurrentHPPercent, InterpTargetHPPercent, InDeltaTime, InterpSpeed);
		
		if (HPBar)
		{
			HPBar->SetPercent(CurrentHPPercent);
		}
	}
}

void UEnemyHPWidget::UpdateHPBar(float NewPercent)
{
	InterpTargetHPPercent = FMath::Clamp(NewPercent, 0.0f, 1.0f);
}
