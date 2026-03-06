// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ControllerHUD.h"
#include "UI/MinimapWidget.h"
#include "UI/SkillSlotWidget.h"
#include "UI/PlayerHPWidget.h"
#include "UI/EnemyHPWidget.h"
#include "UI/DamageIndicatorWidget.h"
#include "UI/GameoverWidget.h"
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
	SetSkillSlotWidget(SkillSlotWidgetClass);
	
	PC = GetWorld()->GetFirstPlayerController();
	
	TScriptInterface<IDamageable> DamageableActor = GetOwningPawn();
	if (DamageableActor)
	{
		SetPlayerHPWidget(PlayerHPWidgetClass, DamageableActor);
	}
}


void AControllerHUD::OverlayMatOnAimCaptured(AActor* Object, AActor* CachedObj, AActor*& Aimed)
{
	if (Object != CachedObj)
	{
		if (IsValid(CachedObj))
		{
			Aimed = nullptr;
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
			Aimed = nullptr;
			if (Object->ActorHasTag(FName("Enemy")))
			{
				Object->FindComponentByClass<USkeletalMeshComponent>()->SetOverlayMaterial(OutlineMat);
				SetEnemyHPWidget(EnemyHPWidgetClass, IDamageable::Execute_GetHPPercent(Object));
				Aimed = Object;
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
	if (NewWidget)
	{
		MinimapWidget = CreateWidget<UMinimapWidget>(GetWorld(), NewWidget);
		if (MinimapWidget)
		{
			MinimapWidget->AddToViewport();
		}
	}
}

void AControllerHUD::SetSkillSlotWidget(TSubclassOf<UUserWidget> NewWidget)
{
	if (NewWidget)
	{
		SkillSlotWidget = CreateWidget<USkillSlotWidget>(GetWorld(), NewWidget);
		if (SkillSlotWidget)
		{
			SkillSlotWidget->AddToViewport();
		}
	}
}

void AControllerHUD::UpdateSkillSlotWidget(int Key, const FString& Skill)
{
	SkillSlotWidget->UpdateSkillCoolTime(Key, Skill);
}

void AControllerHUD::SetPlayerHPWidget(TSubclassOf<UUserWidget> NewWidget, TScriptInterface<IDamageable> Player)
{
	if (NewWidget)
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
	if (NewWidget)
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
	if (EnemyHPWidget)
	{
		EnemyHPWidget->UpdateHPBar(IDamageable::Execute_GetHPPercent(Object));
	}
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
		
		// PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		
		if (PC && PC->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation))
		{
			DamageWidget->SetPositionInViewport(ScreenLocation);
		}
	}
}

void AControllerHUD::Gameover()
{
	GameoverWidget = CreateWidget<UGameoverWidget>(GetWorld(), GameoverWidgetClass);
	if (GameoverWidget)
	{
		GameoverWidget->AddToViewport();
		PC->SetPause(true);
		PC->SetInputMode(FInputModeUIOnly());
		PC->SetShowMouseCursor(true);
	}
}
