// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ControllerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MinimapWidget.h"
#include "UI/PlayerHPWidget.h"
#include "UI/EnemyHPWidget.h"
#include "UI/DamageIndicatorWidget.h"
#include "Blueprint/UserWidget.h"

AControllerHUD::AControllerHUD()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> TempMat(TEXT("M_Outline'/Game/KHJ/Materials/M_Outline.M_Outline'"));
	if (TempMat.Succeeded())
	{
		OutlineMat = TempMat.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> TempMat2(TEXT("M_ShineEffect'/Game/KHJ/Materials/M_ShineEffect.M_ShineEffect'"));
	if (TempMat2.Succeeded())
	{
		ShineMat = TempMat2.Object;
	}
}

void AControllerHUD::BeginPlay()
{
	Super::BeginPlay();
	SetMiniMapWidget(MinimapWidgetClass);
	
	// auto Player = Cast<APlayerCharacter>(GetOwningPawn());
	TScriptInterface<IDamageable> DamageableActor = GetOwningPawn();
	if (DamageableActor)
	{
		SetPlayerHPWidget(PlayerHPWidgetClass, DamageableActor);
	}
}


void AControllerHUD::OverlayMatOnAimCaptured(AActor* Object, AActor* CachedObj)
{
	if (Object != CachedObj)
	{
		if (IsValid(CachedObj))
		{
			if (CachedObj->ActorHasTag(FName("Enemy")))
			{
				CachedObj->FindComponentByClass<USkeletalMeshComponent>()->SetOverlayMaterial(nullptr);
			}
			if (EnemyHPWidget)
			{
				EnemyHPWidget->RemoveFromParent();
			}
			if (CachedObj->ActorHasTag(FName("Item")))
			{
				CachedObj->FindComponentByClass<UMeshComponent>()->SetOverlayMaterial(nullptr);
			}
		}
		if (IsValid(Object))
		{
			if (Object->ActorHasTag(FName("Enemy")))
			{
				Object->FindComponentByClass<USkeletalMeshComponent>()->SetOverlayMaterial(OutlineMat);
				SetEnemyHPWidget(EnemyHPWidgetClass, IDamageable::Execute_GetHPPercent(Object));
			}
			else if (EnemyHPWidget)
			{
				EnemyHPWidget->RemoveFromParent();
			}
			if (Object->ActorHasTag(FName("Item")))
			{
				Object->FindComponentByClass<UMeshComponent>()->SetOverlayMaterial(ShineMat);
			}
		}
	}
}

void AControllerHUD::SetMiniMapWidget(TSubclassOf<UUserWidget> NewWidget)
{
	if (MinimapWidget != nullptr)
	{
		MinimapWidget->RemoveFromParent();
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

void AControllerHUD::SetPlayerHPWidget(TSubclassOf<UUserWidget> NewWidget, TScriptInterface<IDamageable> Player)
{
	if (PlayerHPWidget != nullptr)
	{
		PlayerHPWidget->RemoveFromParent();
		PlayerHPWidget = nullptr;
	}
	if (NewWidget != nullptr)
	{
		PlayerHPWidget = CreateWidget<UPlayerHPWidget>(GetWorld(), PlayerHPWidgetClass);
		if (PlayerHPWidget)
		{
			PlayerHPWidget->InitializeHPBar(Player);
			PlayerHPWidget->AddToViewport();
			
			// EnemyHPWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void AControllerHUD::UpdatePlayerHPWidget(float NewPercent)
{
	PlayerHPWidget->UpdateHPBar(NewPercent);
}

void AControllerHUD::SetEnemyHPWidget(TSubclassOf<UUserWidget> NewWidget, float NewPercent)
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
			EnemyHPWidget->UpdateHPBar(NewPercent);
			EnemyHPWidget->AddToViewport();
		}
	}
}

void AControllerHUD::UpdateEnemyHPWidget(AActor* Object)
{
	EnemyHPWidget->UpdateHPBar(IDamageable::Execute_GetHPPercent(Object));
}

void AControllerHUD::DrawDamageWidget(FVector WorldLocation, int DamageAmount)
{
	if (!DamageWidgetClass)
	{
		return;
	}
	
	DamageWidget = CreateWidget<UDamageIndicatorWidget>(GetWorld(), DamageWidgetClass);
	
	if (DamageWidget)
	{
		DamageWidget->SetDamageValue(DamageAmount);
		DamageWidget->AddToViewport();
		
		FVector2D ScreenLocation;
		
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC && PC->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation))
		{
			DamageWidget->SetPositionInViewport(ScreenLocation);
		}
	}
}
