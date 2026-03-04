// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHPWidget.h"


void UPlayerHPWidget::InitializeHPBar(TScriptInterface<IDamageable> InTarget)
{
	if (InTarget)
	{
		float CurrentHP = InTarget->Execute_GetHP(InTarget.GetObject());
		float MaxHP = InTarget->Execute_GetHPPercent(InTarget.GetObject());
		UpdateHPBar(CurrentHP, MaxHP);
		
		// 2. 델리게이트 바인딩
		// TScriptInterface에서 인터페이스 포인터를 꺼내올 때는 GetInterface()
		// InTarget->GetOnHPChangedDelegate().AddUObject(this, &UPlayerHPWidget::UpdateHPBar);
	}
}

void UPlayerHPWidget::UpdateHPBar(int CurrentHP, int MaxHP)
{
	if (HPBar)
	{
		float NewPercent = CurrentHP / MaxHP;
		HPBar->SetPercent(NewPercent);
	}
}
