// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC.generated.h"

// --- 1. NPC의 상태를 정의하는 열거형 ---
UENUM(BlueprintType)
enum class ENPCState : uint8
{
	Idle    UMETA(DisplayName = "Idle"),
	Follow  UMETA(DisplayName = "Follow"),
	Combat  UMETA(DisplayName = "Combat")
};

UCLASS()
class HOGWARTSLEGACY_API ANPC : public ACharacter
{
	GENERATED_BODY()

public:
	ANPC();

protected:
	virtual void BeginPlay() override;

public: 
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
	// --- 2. 상태 관련 변수 및 함수 ---
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|State")
	ENPCState CurrentState = ENPCState::Idle;

	UFUNCTION(BlueprintCallable, Category = "AI|State")
	void SetNPCState(ENPCState NewState);

	// --- 3. 기본 변수들 ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MovementSpeed = 450.0f;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Support AI")
	class ACharacter* PlayerRef;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float FollowDistance = 80.0f;

	// --- 4. 전투 관련 추가 변수 및 함수 ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float DetectionRadius = 800.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	AActor* TargetEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float AttackRange = 250.0f;
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	void ClearTargetEnemy();
};