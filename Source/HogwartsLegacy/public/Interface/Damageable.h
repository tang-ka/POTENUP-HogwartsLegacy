// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Damage")
	TObjectPtr<AActor> DamageCauser{};

	UPROPERTY(BlueprintReadOnly, Category="Damage")
	float DamageAmount{};
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HOGWARTSLEGACY_API IDamageable
{
	GENERATED_BODY()
	
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * Applies damage to the implementing actor.
	 *
	 * @param DamageInfo  Information describing the damage to apply (e.g., amount, causer).
	 * @return True if damage was applied successfully; false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Damage")
	bool ReceiveDamage (FDamageInfo DamageInfo);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Damage")
	float GetHP() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Damage")
	bool IsDead() const;
};
