// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MinimapUserWidget.h"
#include "Character/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

void UMinimapUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	APawn* PlayerPawn = GetOwningPlayerPawn();
	GetController()->GetPlayerViewPoint(DummyLocation, DummyRotation);
	if (PlayerPawn && MinimapTrim)
	{
		float PlayerYaw = PlayerPawn->GetActorRotation().Yaw;

		// 3. Apply it to the Render Transform
		// Note: You might need to add or subtract 90/180 degrees 
		// depending on how your texture is oriented.
		MinimapTrim->SetRenderAngle(PlayerYaw);
	}
}

void UMinimapUserWidget::SetPlayerRotation()
{
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	UKismetMathLibrary::MapRangeClamped();
}
