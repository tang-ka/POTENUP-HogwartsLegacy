

#include "UI/DamageIndicatorWidget.h"
#include "Components/TextBlock.h"


void UDamageIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (GoingUpFadeOut)
	{
		PlayDamageAnimation(GoingUpFadeOut);
	}
}

void UDamageIndicatorWidget::SetDamageValue(int DamageAmount)
{
	if (DamageNumberText)
	{
		DamageNumberText->SetText(FText::AsNumber(DamageAmount));
	}
}

void UDamageIndicatorWidget::PlayDamageAnimation(UWidgetAnimation* AppearAnim)
{
	if (AppearAnim)
	{
		PlayAnimation(AppearAnim);
	}
	else
	{
		RemoveFromParent();
	}
}

void UDamageIndicatorWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);
	RemoveFromParent();
}
