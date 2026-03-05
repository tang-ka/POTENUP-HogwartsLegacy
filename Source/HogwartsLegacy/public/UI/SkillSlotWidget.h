
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "SkillSlotWidget.generated.h"

class UCanvasPanel;
class UImage;
class UProgressBar;

/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API USkillSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UCanvasPanel* SkillSlotPanel;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* Key1;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* Key2;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* Key3;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* Key4;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* Slot1;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* Slot2;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* Slot3;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* Slot4;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UProgressBar* Slot1BackOverlay;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* Slot1Background;
	
	UFUNCTION()
	void UpdateSkillCoolTime(int InKey, const FString& InSkillName);
	
	float KeyXMargin = 80.f;
	float SlotXMargin = 80.f;
	
	float DeltaSeconds = 9999.f;
	float SkillCoolTime = 100.f;
	float CoolTimePercent;
	int Key;
	
	float BasicAttackCoolTime = 0.f;
	float LumosCoolTime = 1.0f;
	float LeviosoCoolTime = 8.0f;
	float DescendoCoolTime = 10.0f;
};
