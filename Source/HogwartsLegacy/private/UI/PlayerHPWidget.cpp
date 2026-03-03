// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHPWidget.h"


void UPlayerHPWidget::InitializeHPBar(TScriptInterface<IDamageable> InTarget)
{
	if (InTarget)
	{
		float NewHP = InTarget->Execute_GetHPPercent(InTarget.GetObject());
		UpdateHPBar(NewHP);
	}
}

void UPlayerHPWidget::UpdateHPBar(float NewPercent)
{
	if (HPBar)
	{
		HPBar->SetPercent(NewPercent);
	}
}
