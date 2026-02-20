// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellBase.generated.h"

UCLASS()
class HOGWARTSLEGACY_API ASpellBase : public AActor
{
	GENERATED_BODY()
    
public:    
	ASpellBase();

protected:
	virtual void BeginPlay() override;

public:    
	virtual void Tick(float DeltaTime) override;

	// --- 컴포넌트 ---
	// 발사체의 물리 이동을 담당
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovement;

	// 충돌 범위를 결정
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent;

	// --- 설계하신 데미지 변수 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell Stats")
	float BaseDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell Stats")
	float SpellSpeed = 3000.0f;
};
