// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ControllerHUD.h"
#include "UI/MinimapWidget.h"
#include "UI/EnemyHPWidget.h"

AControllerHUD::AControllerHUD()
{
	ConstructorHelpers::FClassFinder<UMinimapWidget> MiniMapWidgetAsset(TEXT("/Game/KHJ/HUD/WBP_HUD_MiniMap_C"));
	if (MiniMapWidgetAsset.Succeeded())
	{
		MinimapWidgetClass = MiniMapWidgetAsset.Class;
	}
	ConstructorHelpers::FClassFinder<UEnemyHPWidget> UEnemyHPWidgetAsset(TEXT("/Game/KHJ/HUD/WBP_EnemyHPWidget_C"));
	if (UEnemyHPWidgetAsset.Succeeded())
	{
		EnemyHPWidgetClass = UEnemyHPWidgetAsset.Class;
	}
}

void AControllerHUD::BeginPlay()
{
	Super::BeginPlay();
	SetMiniMapWidget(MinimapWidgetClass);
}

void AControllerHUD::SetMiniMapWidget(TSubclassOf<UUserWidget> NewWidget)
{
	if (MinimapWidget != nullptr)
	{
		MinimapWidget->RemoveFromParent(); // RemoveFromViewport()
		MinimapWidget = nullptr;
	}
	if (NewWidget != nullptr)
	{
		MinimapWidget = CreateWidget<UMinimapWidget>(GetWorld(), NewWidget);
		if (MinimapWidget != nullptr)
		{
			MinimapWidget->AddToViewport();
		}
	}
}

void AControllerHUD::SetEnemyHPWidget(TSubclassOf<UUserWidget> NewWidget)
{
	if (EnemyHPWidget != nullptr)
	{
		EnemyHPWidget->RemoveFromParent();
		EnemyHPWidget = nullptr;
	}
	if (NewWidget != nullptr)
	{
		EnemyHPWidget = CreateWidget<UEnemyHPWidget>(GetWorld(), EnemyHPWidgetClass);
		if (EnemyHPWidget)
		{
			EnemyHPWidget->AddToViewport();
			EnemyHPWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
