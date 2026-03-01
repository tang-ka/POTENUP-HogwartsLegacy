// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MinimapWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MinimapWorldRadius = DefaultWorldRadius;
	TargetWorldRadius = DefaultWorldRadius;
}

void UMinimapWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (MinimapCanvas)
	{
		MinimapCanvas->SetRenderTransformPivot(FVector2D(0, 1));
	}
	
	if (MinimapTrim && PlayerMarker)
	{
		UCanvasPanelSlot* TrimSlot = Cast<UCanvasPanelSlot>(MinimapTrim->Slot);
		UCanvasPanelSlot* PlayerSlot = Cast<UCanvasPanelSlot>(PlayerMarker->Slot);

		if (TrimSlot && PlayerSlot)
		{
			PlayerSlot->SetAnchors(TrimSlot->GetAnchors());
			PlayerSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			PlayerSlot->SetPosition(FVector2D(0.f, 0.f));
		}
	}
}

void UMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	PlayerController = GetOwningPlayer();
	PlayerPawn = GetOwningPlayerPawn();
	
	if (!PlayerController || !PlayerPawn || !MinimapCanvas || !MinimapTrim || !PlayerMarker) return;
	
	float ControlYaw = PlayerController->GetControlRotation().Yaw;
	MinimapTrim->SetRenderTransformAngle(ControlYaw);
	PlayerMarker->SetRenderTransformAngle(PlayerPawn->GetActorRotation().Yaw);
	
	CurrentZoomAlpha = FMath::FInterpTo(CurrentZoomAlpha, TargetZoomAlpha, InDeltaTime, ZoomInterpSpeed);
	MinimapWorldRadius = FMath::FInterpTo(MinimapWorldRadius, TargetWorldRadius, InDeltaTime, ZoomInterpSpeed);
	
	MinimapCanvas->SetRenderScale(FVector2D(FMath::Lerp(1.0f, MaxCanvasScale, CurrentZoomAlpha)));
	MinimapCanvas->SetRenderTranslation(FMath::Lerp(FVector2D::ZeroVector, MaxCanvasTranslation, CurrentZoomAlpha));
	
	MinimapWorldRadius = FMath::FInterpTo(MinimapWorldRadius, TargetWorldRadius, InDeltaTime, ZoomInterpSpeed);
	
	UpdateEnemyMarkers(ControlYaw, PlayerPawn->GetActorLocation());
}

void UMinimapWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	TargetWorldRadius = ZoomedWorldRadius;
	TargetZoomAlpha = 1.0f;
}

void UMinimapWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	TargetWorldRadius = DefaultWorldRadius;
	TargetZoomAlpha = 0.0f;
}

UUserWidget* UMinimapWidget::GetMarkerFromPool()
{
	UUserWidget* SelectedWidget = nullptr;

	if (WidgetPool.Num() > 0)
	{
		// 풀에서 마지막 요소를 꺼냄
		SelectedWidget = WidgetPool.Pop();
	}
	else
	{
		if (!EnemyMarkerClass) return nullptr;
		SelectedWidget = CreateWidget<UUserWidget>(this, EnemyMarkerClass);
     
		if (SelectedWidget && MinimapCanvas)
		{
			UCanvasPanelSlot* NewSlot = MinimapCanvas->AddChildToCanvas(SelectedWidget);
			UCanvasPanelSlot* PlayerSlot = Cast<UCanvasPanelSlot>(PlayerMarker->Slot);
            
			if (NewSlot && PlayerSlot)
			{
				// 1. 플레이어의 앵커를 그대로 복사
				NewSlot->SetAnchors(PlayerSlot->GetAnchors());
                
				// 2. 플레이어의 에디터 상 위치를 그대로 복사
				NewSlot->SetPosition(PlayerSlot->GetPosition());
                
				// 3. Marker의 중심이 좌표가 되도록 설정
				NewSlot->SetAlignment(FVector2D(0.5f, 0.5f));
				NewSlot->SetAutoSize(true);
			}
		}
	}

	if (SelectedWidget)
	{
		SelectedWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	return SelectedWidget;
}

void UMinimapWidget::ReturnMarkerToPool(AActor* EnemyActor, UUserWidget* ArrowWidget)
{
	if (!IsValid(ArrowWidget) || WidgetPool.Contains(ArrowWidget)) // 유효성 확인, 이미 풀에 들어가 있는지 확인
	{
		return;
	}
	// 화면에서 숨기고 풀에 추가
	ArrowWidget->SetVisibility(ESlateVisibility::Collapsed);
	WidgetPool.Add(ArrowWidget);
}

void UMinimapWidget::UpdateEnemyMarkers(float yaw, FVector Location)
{
	UCanvasPanelSlot* PlayerSlot = Cast<UCanvasPanelSlot>(PlayerMarker->Slot);
    
	if (!PlayerSlot) return;
    
	// 매 틱마다 검출하고 있으므로 이벤트 드리븐으로 바꿔야 함
	// 에너미가 생기면 TArray에 넣고 죽으면 빼는 식으로
    TArray<AActor*> FoundEnemies;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), FoundEnemies);
    
    // 더 이상 존재하지 않는 적 제거
    TArray<AActor*> ActorsToRemove;
    for (auto& Pair : ActiveMarkers)
    {
       if (!FoundEnemies.Contains(Pair.Key) || !IsValid(Pair.Key))
       {
          ActorsToRemove.Add(Pair.Key);
       }
    }

    for (AActor* Actor : ActorsToRemove)
    {
       ReturnMarkerToPool(Actor, ActiveMarkers[Actor]);
       ActiveMarkers.Remove(Actor);
    }
    
    // 적 위치 업데이트 루프
	for (AActor* Enemy : FoundEnemies)
	{
		if (!Enemy)
		{
			continue;
		}

		if (!ActiveMarkers.Contains(Enemy))
		{
			ActiveMarkers.Add(Enemy, GetMarkerFromPool());
		}
       
		UUserWidget* Arrow = ActiveMarkers[Enemy];
		
		if (!IsValid(Arrow))
		{
			continue;
		}

		if (UCanvasPanelSlot* ArrowSlot = Cast<UCanvasPanelSlot>(Arrow->Slot))
		{
			// 적 화살표의 앵커를 플레이어 위치(비율)로 설정, 화살표의 (0,0) 지점은 플레이어 마커의 중심.
			ArrowSlot->SetAnchors(PlayerSlot->GetAnchors());
			
			FVector RelativePos = Enemy->GetActorLocation() - Location;
			FRotator ViewRotation(0.f, yaw, 0.f);
			RelativePos = ViewRotation.UnrotateVector(RelativePos);

			float Distance2D = RelativePos.Size2D();
			float ClampedDistance = FMath::Min(Distance2D * MinimapRadius / MinimapWorldRadius, MinimapRadius);
			FVector2D Direction = FVector2D(RelativePos.X, RelativePos.Y).GetSafeNormal();
			
			FVector2D RelativeUIOffset = FVector2D(Direction.Y * ClampedDistance, -Direction.X * ClampedDistance);
          
			ArrowSlot->SetPosition(RelativeUIOffset);
			Arrow->SetRenderTransformAngle(Enemy->GetActorRotation().Yaw);
            
			Arrow->SetRenderOpacity(Distance2D > MinimapWorldRadius ? 0.5f : 1.0f);
		}
	}
}

