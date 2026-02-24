// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/Damageable.h"
#include "Guardian.generated.h"

class UEnemyFSMComponent;

UCLASS()
class HOGWARTSLEGACY_API AGuardian : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	AGuardian();

public:	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
#pragma region IDamageable
	virtual bool ReceiveDamage_Implementation(FDamageInfo DamageInfo) override;
	virtual float GetHP_Implementation() const override;
	virtual bool IsDead_Implementation() const override;
#pragma endregion
	
public:
#pragma region Test
	void SetMaterialColor(FLinearColor Color);
	void ResetMaterialColor();
#pragma endregion
	
	float GetCurrentHP() const { return HP; }
	UEnemyFSMComponent* GetFSMComponent() const { return FSMComponent; }
	AActor* GetTargetActor() const { return TargetActor; }
	
private:
	void InitHP();
	
private:
	int HP{};
	int MaxHP{100};
	
private:
	UPROPERTY(EditAnywhere, Category="FSMComponent")
	TObjectPtr<UEnemyFSMComponent> FSMComponent;

	UPROPERTY(EditAnywhere, Category="Target")
	TObjectPtr<AActor> TargetActor;
	
	FLinearColor DefaultColor;
};