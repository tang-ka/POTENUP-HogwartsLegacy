

#include "UI/EnemyHPWidget.h"


void UEnemyHPWidget::UpdateHPBar(float NewPercent)
{
	if (HPBar)
	{
		HPBar->SetPercent(NewPercent);
	}
}
