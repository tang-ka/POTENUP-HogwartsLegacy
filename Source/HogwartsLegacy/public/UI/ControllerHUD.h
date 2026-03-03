// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Character/PlayerCharacter.h"
#include "ControllerHUD.generated.h"

class UMinimapWidget;
class UPlayerHPWidget;
class UEnemyHPWidget;
class UDamageIndicatorWidget;
/**
 * 
 */
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamaged, FVector, WorldLocation, int, DamageAmount);

UCLASS()
class HOGWARTSLEGACY_API AControllerHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	AControllerHUD();
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf <UMinimapWidget> MinimapWidgetClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf <UPlayerHPWidget> PlayerHPWidgetClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf <UEnemyHPWidget> EnemyHPWidgetClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf <UDamageIndicatorWidget> DamageWidgetClass;
	UPROPERTY()
	UMinimapWidget* MinimapWidget;
	UPROPERTY()
	UPlayerHPWidget* PlayerHPWidget;
	UPROPERTY()
	UEnemyHPWidget* EnemyHPWidget;
	UPROPERTY()
	UDamageIndicatorWidget* DamageWidget;
	UPROPERTY()
	TObjectPtr <UMaterialInterface> OutlineMat;
	UPROPERTY()
	TObjectPtr <UMaterialInterface> ShineMat;
	UFUNCTION()
	void OverlayMatOnAimCaptured(AActor* Object, AActor* CachedObj);
	
public:
	UFUNCTION(BlueprintCallable)
	void SetMiniMapWidget(TSubclassOf<UUserWidget> NewWidget);
	UFUNCTION(BlueprintCallable)
	void SetPlayerHPWidget(TSubclassOf<UUserWidget> NewWidget, TScriptInterface<IDamageable> Player);
	UFUNCTION(BlueprintCallable)
	void SetEnemyHPWidget(TSubclassOf<UUserWidget> NewWidget, float NewPercent);
	//UFUNCTION(BlueprintCallable)
	//FOnDamaged OnDamaged;
	UFUNCTION(BlueprintCallable)
	void DrawDamageWidget(FVector WorldLocation, int DamageAmount);
	
	// ShowDamageWidgetDelegate;
	// TSharedRef <FOnDamaged> OnDamaged(new FOnDamaged());
};
