

#include "UI/SkillSlotWidget.h"


void USkillSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	// UCanvasPanelSlot* Key1Temp = Cast<UCanvasPanelSlot>(Key1->Slot);
	// FAnchors FAnchorsAnchor = Key1Temp->GetAnchors();
	// Key1Temp->SetAlignment(FVector2D(0.5f, 0.5f));
	// Key1Temp->SetOffsets(FMargin(0.f, 0.f, 0.f, 0.f));
	// Key1Temp->SetZOrder(4);
	//
	// UCanvasPanelSlot* Key2Temp = Cast<UCanvasPanelSlot>(Key2->Slot);
	// Key2Temp->SetAnchors(FAnchors(FAnchorsAnchor.Maximum.X+SlotXMargin, FAnchorsAnchor.Maximum.Y));
	// Key2Temp->SetAlignment(FVector2D(0.5f, 0.5f));
	// Key2Temp->SetOffsets(FMargin(0.f, 0.f, 0.f, 0.f));
	// Key2Temp->SetZOrder(4);
	//
	// UCanvasPanelSlot* Key3Temp = Cast<UCanvasPanelSlot>(Key3->Slot);
	// Key3Temp->SetAnchors(FAnchors(FAnchorsAnchor.Maximum.X+SlotXMargin*2, FAnchorsAnchor.Maximum.Y));
	// Key3Temp->SetAlignment(FVector2D(0.5f, 0.5f));
	// Key3Temp->SetOffsets(FMargin(0.f, 0.f, 0.f, 0.f));
	// Key3Temp->SetZOrder(4);
	//
	// UCanvasPanelSlot* Key4Temp = Cast<UCanvasPanelSlot>(Key4->Slot);
	// Key4Temp->SetAnchors(FAnchors(FAnchorsAnchor.Maximum.X+SlotXMargin*3, FAnchorsAnchor.Maximum.Y));
	// Key4Temp->SetAlignment(FVector2D(0.5f, 0.5f));
	// Key4Temp->SetOffsets(FMargin(0.f, 0.f, 0.f, 0.f));
	// Key4Temp->SetZOrder(4);
	//
	// UCanvasPanelSlot* Slot1Temp = Cast<UCanvasPanelSlot>(Slot1->Slot);
	// FAnchors Slot1Anchor = Slot1Temp->GetAnchors();
	// Slot1Temp->SetAlignment(FVector2D(0.5f, 0.5f));
	// Slot1Temp->SetOffsets(FMargin(0.f, 0.f, 0.f, 0.f));
	// Slot1Temp->SetZOrder(2);
	// Slot1->SetRenderTransformAngle(135);
	//
	// UCanvasPanelSlot* Slot2Temp = Cast<UCanvasPanelSlot>(Slot2->Slot);
	// Slot2Temp->SetAnchors(FAnchors(Slot1Anchor.Maximum.X+SlotXMargin, Slot1Anchor.Maximum.Y));
	// Slot2Temp->SetAlignment(FVector2D(0.5f, 0.5f));
	// Slot2Temp->SetOffsets(FMargin(0.f, 0.f, 0.f, 0.f));
	// Slot2Temp->SetZOrder(2);
	// Slot2->SetRenderTransformAngle(135);
	//
	// UCanvasPanelSlot* Slot3Temp = Cast<UCanvasPanelSlot>(Slot3->Slot);
	// Slot3Temp->SetAnchors(FAnchors(Slot1Anchor.Maximum.X+SlotXMargin*2, Slot1Anchor.Maximum.Y));
	// Slot3Temp->SetAlignment(FVector2D(0.5f, 0.5f));
	// Slot3Temp->SetOffsets(FMargin(0.f, 0.f, 0.f, 0.f));
	// Slot3Temp->SetZOrder(2);
	// Slot3->SetRenderTransformAngle(135);
	//
	// UCanvasPanelSlot* Slot4Temp = Cast<UCanvasPanelSlot>(Slot4->Slot);
	// Slot4Temp->SetAnchors(FAnchors(Slot1Anchor.Maximum.X+SlotXMargin*3, Slot1Anchor.Maximum.Y));
	// Slot4Temp->SetAlignment(FVector2D(0.5f, 0.5f));
	// Slot4Temp->SetOffsets(FMargin(0.f, 0.f, 0.f, 0.f));
	// Slot4Temp->SetZOrder(2);
	// Slot4->SetRenderTransformAngle(135);
	//
	// UCanvasPanelSlot* Slot1BackOverlayTemp = Cast<UCanvasPanelSlot>(Slot1BackOverlay->Slot);
	// Slot1BackOverlayTemp->SetAnchors(Slot1Anchor);
	// Slot1BackOverlayTemp->SetAlignment(FVector2D(0.5f, 0.5f));
	// Slot1BackOverlayTemp->SetOffsets(FMargin(0.f, 0.f, 0.f, 0.f));
	// Slot1BackOverlayTemp->SetZOrder(1);
	//
	// UCanvasPanelSlot* Slot1BackgroundTemp = Cast<UCanvasPanelSlot>(Slot1Background->Slot);
	// Slot1BackgroundTemp->SetAnchors(Slot1Anchor);
	// Slot1BackgroundTemp->SetAlignment(FVector2D(0.5f, 0.5f));
	// Slot1BackgroundTemp->SetOffsets(FMargin(0.f, 0.f, 0.f, 0.f));
	// Slot1BackgroundTemp->SetZOrder(0);
	
}

void USkillSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	DeltaSeconds += InDeltaTime;
	CoolTimePercent = DeltaSeconds / SkillCoolTime;
	if (DeltaSeconds < SkillCoolTime)
	{
		switch (Key)
		{
			case 1: Slot1BackOverlay->SetPercent(CoolTimePercent);
		}
	}
}

void USkillSlotWidget::UpdateSkillCoolTime(int InKey, const FString& InSkillName)
{
	Key = InKey;
	
	if (InSkillName == FString(TEXT("Lumos")))
	{
		SkillCoolTime = LumosCoolTime;
	}
	DeltaSeconds = 0.f;
}
