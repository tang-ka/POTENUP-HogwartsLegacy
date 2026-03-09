

#include "HogwartsLegacyPlayerController.h"

void AHogwartsLegacyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameAndUI());
}
