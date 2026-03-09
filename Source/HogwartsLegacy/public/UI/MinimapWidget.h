
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinimapWidget.generated.h"

class UCanvasPanel;
class UImage;
/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UMinimapWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	UPROPERTY()
	APlayerController* PlayerController;
	UPROPERTY()
	APawn* PlayerPawn;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UCanvasPanel* MinimapCanvas;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* MinimapTrim;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* PlayerMarker;
	
	UPROPERTY()
	TArray<UUserWidget*> WidgetPool;
	UPROPERTY()
	TMap<AActor*, UUserWidget*> ActiveMarkers;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> EnemyMarkerClass;
	
	UFUNCTION()
	UUserWidget* GetMarkerFromPool();
	UFUNCTION()
	void ReturnMarkerToPool(AActor* EnemyActor, UUserWidget* ArrowWidget);
	UFUNCTION()
	void UpdateEnemyMarkers(float yaw, FVector Location);
	
	UPROPERTY(EditAnywhere)
	float MinimapWorldRadius;
	UPROPERTY(EditAnywhere)
	float MinimapRadius = 122.5f;
	UPROPERTY(EditAnywhere)
	float DefaultWorldRadius = 2000.0f;
	UPROPERTY(EditAnywhere)
	float TargetWorldRadius;
	
	UPROPERTY(EditAnywhere)
	float ZoomedWorldRadius = 1000.f;
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 5.0f;
	
	UPROPERTY(EditAnywhere)
	FVector2D ZoomOffset = FVector2D(50.0f, -35.0f); // 오른쪽으로 50, 위로 50 (UI는 위가 -Y)

	float CurrentZoomAlpha = 0.0f; 
	float TargetZoomAlpha = 0.0f;

	UPROPERTY(EditAnywhere)
	float MaxCanvasScale = 1.5f;
	UPROPERTY(EditAnywhere)
	FVector2D MaxCanvasTranslation = FVector2D(50.0f, -35.0f);
};
