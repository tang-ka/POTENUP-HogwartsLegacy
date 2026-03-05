
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Character/PlayerCharacter.h"
#include "ControllerHUD.generated.h"

class UMinimapWidget;
class USkillSlotWidget;
class UPlayerHPWidget;
class UEnemyHPWidget;
class UDamageIndicatorWidget;
class UGameoverWidget;
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
	
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf <UMinimapWidget> MinimapWidgetClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf <USkillSlotWidget> SkillSlotWidgetClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf <UPlayerHPWidget> PlayerHPWidgetClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf <UEnemyHPWidget> EnemyHPWidgetClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf <UDamageIndicatorWidget> DamageWidgetClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf <UGameoverWidget> GameoverWidgetClass;
	UPROPERTY()
	UMinimapWidget* MinimapWidget;
	UPROPERTY()
	USkillSlotWidget* SkillSlotWidget;
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
	UPROPERTY()
	UGameoverWidget* GameoverWidget;
	UPROPERTY()
	APlayerController* PC;
	
	UFUNCTION(BlueprintCallable)
	void SetMiniMapWidget(TSubclassOf<UUserWidget> NewWidget);
	UFUNCTION(BlueprintCallable)
	void SetSkillSlotWidget(TSubclassOf<UUserWidget> NewWidget);
	UFUNCTION(BlueprintCallable)
	void UpdateSkillSlotWidget(int Key, const FString& Skill);
	UFUNCTION(BlueprintCallable)
	void SetPlayerHPWidget(TSubclassOf<UUserWidget> NewWidget, TScriptInterface<IDamageable> Player);
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerHPWidget(float NewPercent);
	UFUNCTION(BlueprintCallable)
	void SetEnemyHPWidget(TSubclassOf<UUserWidget> NewWidget, float NewPercent);
	UFUNCTION(BlueprintCallable)
	void UpdateEnemyHPWidget(AActor* Object);
	UFUNCTION(BlueprintCallable)
	void OverlayMatOnAimCaptured(AActor* Object, AActor* CachedObj, AActor*& Aimed);
	UFUNCTION(BlueprintCallable)
	void DrawDamageWidget(FVector WorldLocation, int DamageAmount);
	UFUNCTION(BlueprintCallable)
	void Gameover();
	
	
};
