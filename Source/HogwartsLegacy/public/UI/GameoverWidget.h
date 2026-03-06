
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameoverWidget.generated.h"

class UCanvasPanel;
class UImage;
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
	UImage* GameoverImage;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UButton* TryAgainButton;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UButton* QuitButton;
	
public:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnTryAgain();
	
	UFUNCTION()
	void OnQuit();
};
