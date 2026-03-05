
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HogwartsLegacyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API AHogwartsLegacyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
};
