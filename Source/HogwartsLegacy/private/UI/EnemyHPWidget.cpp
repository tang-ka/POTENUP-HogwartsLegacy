// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EnemyHPWidget.h"


void UEnemyHPWidget::UpdateHPBar(float NewPercent)
{
	HPBar->SetPercent(NewPercent);
}
