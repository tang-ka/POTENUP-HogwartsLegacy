// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ControllerHUD.h"
#include "UI/MinimapWidget.h"
#include "UI/PlayerHPWidget.h"
#include "UI/EnemyHPWidget.h"

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
			if (CachedObj->ActorHasTag(FName("Item")))
			{
				CachedObj->FindComponentByClass<UMeshComponent>()->SetOverlayMaterial(nullptr);
			}
		}
		if (IsValid(Object))
		{
			// GEngine->AddOnScreenDebugMessage(3, 1.0f, FColor::Green, Obj->GetName());
			if (Object->ActorHasTag(FName("Enemy")))
			{
				Object->FindComponentByClass<USkeletalMeshComponent>()->SetOverlayMaterial(OutlineMat);
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
			// float Percent = Player->GetHP_Implementation() / Player->GetMaxHP_Implementation();
			// GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Green, FString::FromInt(Percent));
			PlayerHPWidget->InitializeHPBar(Player);
			PlayerHPWidget->AddToViewport();
			
			// EnemyHPWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
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
			// EnemyHPWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
