
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameoverWidget.generated.h"

class UCanvasPanel;
class UButton;
/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UGameoverWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UCanvasPanel* GameoverPanel;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UButton* TryAgainButton;
	
public:
	void NativeConstruct() override;
	void OnTryAgain();
	void OnQuit();
	
};
