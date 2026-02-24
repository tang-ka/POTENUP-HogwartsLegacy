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
    Combat  UMETA(DisplayName = "Combat"),
    Damaged  UMETA(DisplayName = "Damaged"),
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

    // --- 3. 기본 능력치 및 이동 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MovementSpeed = 450.0f;

    // 공격력 변수 (블루프린트에서 수정 가능)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float BaseDamage = 20.0f;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Support AI")
    class ACharacter* PlayerRef;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float FollowDistance = 80.0f;

    // --- 4. 전투 관련 변수 및 함수 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float DetectionRadius = 800.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    AActor* TargetEnemy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float AttackRange = 250.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    bool bCanAttack = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float AttackCooldown = 2.0f;
    
    UFUNCTION(BlueprintCallable, Category = "AI")
    void ClearTargetEnemy();

    // --- 5. 데미지 처리 함수 (엔진 표준 시스템 오버라이드) ---
    // 이 함수가 있어야 "맞았을 때" 반격하는 로직을 짤 수 있습니다.
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};