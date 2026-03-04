
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageIndicatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UDamageIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DamageNumberText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* GoingUpFadeOut;
	
	UFUNCTION()
	void SetDamageValue(int DamageAmount);
	
	UFUNCTION()
	void PlayDamageAnimation(UWidgetAnimation* AppearAnim);
	
	
};
