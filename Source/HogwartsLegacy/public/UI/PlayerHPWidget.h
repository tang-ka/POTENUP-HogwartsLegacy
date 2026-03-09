
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Interface/Damageable.h"
#include "PlayerHPWidget.generated.h"

class UCanvasPanel;
class UProgressBar;
/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UPlayerHPWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UCanvasPanel* HPBarPanel;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UProgressBar* HPBar;
	
public:
	void InitializeHPBar(TScriptInterface<IDamageable> InTarget);
	void UpdateHPBar(float NewPercent);
	
private:
	float CurrentHPPercent = 1.0f;
	
};
