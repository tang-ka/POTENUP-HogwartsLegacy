// Fill out your copyright notice in the Description page of Project Settings.


#include "HogwartsLegacyGameModeBase.h"
#include "HogwartsLegacy.h"

AHogwartsLegacyGameModeBase::AHogwartsLegacyGameModeBase()
{
	SetMiniMapWidget(Widgets);
}

void AHogwartsLegacyGameModeBase::SetMiniMapWidget(TSubclassOf<UUserWidget> NewWidget)
{
	if (MinimapWidget != nullptr)
	{
		MinimapWidget->RemoveFromViewport();
		MinimapWidget = nullptr;
	}
	if (NewWidget != nullptr)
	{
		MinimapWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidget);
		if (MinimapWidget != nullptr)
		{
			MinimapWidget->AddToViewport();
		}
	}
}
