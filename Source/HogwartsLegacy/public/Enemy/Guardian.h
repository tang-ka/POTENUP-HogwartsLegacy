// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Guardian.generated.h"

class UEnemyFSMComponent;

UCLASS()
class HOGWARTSLEGACY_API AGuardian : public ACharacter
{
	GENERATED_BODY()

public:
	AGuardian();

public:	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	void SetMaterialColor(FLinearColor Color);
	void ResetMaterialColor();

	UEnemyFSMComponent* GetFSMComponent() const { return FSMComponent; }
	AActor* GetTargetActor() const { return TargetActor; }
	
public:
	int HP;
	int MaxHP;
	
private:
	UPROPERTY(EditAnywhere, Category="FSMComponent")
	TObjectPtr<UEnemyFSMComponent> FSMComponent;

	UPROPERTY(EditAnywhere, Category="Target")
	TObjectPtr<AActor> TargetActor;
	
	FLinearColor DefaultColor;
};