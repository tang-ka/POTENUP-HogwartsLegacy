// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MinimapUserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UMinimapUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//
	// if (MainCanvas)
	// {
	// 	if (MinimapTrim && PlayerArrow)
	// 	{
	// 		MainCanvas->AddChildToCanvas(MinimapTrim);
	// 		MainCanvas->AddChildToCanvas(PlayerArrow);
	// 	}
	// }
}

void UMinimapUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	PlayerController = GetOwningPlayer();
	PlayerPawn = GetOwningPlayerPawn();

	if (PlayerController && MinimapTrim && PlayerPawn && PlayerArrow)
	{
		MinimapTrim->SetRenderTransformAngle(PlayerController->GetControlRotation().Yaw);
		PlayerArrow->SetRenderTransformAngle(PlayerPawn->GetActorRotation().Yaw);
	}
}

void UMinimapUserWidget::FindEnemy()
{
	// UGameplayStatics::GetAllActorsOfClass(ACharacter*);
}