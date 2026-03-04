#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GuardianSettings.generated.h"

UCLASS(Config=Game, defaultconfig, meta=(DisplayName="Guardian Settings"))
class HOGWARTSLEGACY_API UGuardianSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category="Guardian")
	TSoftClassPtr<class AGuardian> GuardianClass;
};

