// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HogwartsLegacy.h"
#include "Enemy/EnemyFSMComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "GuardianManager.generated.h"

class AEnemySpawnPoint;
class AGuardian;

class SpawnPointGenerator
{
public:
	 void Generate(UWorld* World, TArray<TObjectPtr<AEnemySpawnPoint>>& OutSpawnPoints) const;

private:
	int32 SpawnPointCount{6};
	float SpawnRadius{1500.f};
	FVector CenterLocation{0.f, 0.f, 0.f};
};

/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UGuardianManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:	
	void Enable();
	void Disable();
	bool IsEnabled() const { return bIsEnabled; }
	
	void StartCheckTick();
	void CheckTick();
	
	// 공격 독점 관리 — Manager가 주도적으로 공격자를 선택·지시한다
	// 공격 가능한 Guardian 중 가장 적합한 1명을 선택하여 공격 명령
	void SelectAndCommandAttack();
	// Guardian의 공격 종료/사망 시 호출되는 콜백 (공격권 반환)
	void OnGuardianFinishedAttack();
	
	// 스폰해야 할 가디언이 있는지 일정 시간 마다 검사한다.
	int32 GetCurrentAliveCount() const { return AliveGuardians.Num(); }
	
public:
	FTimerHandle SpawnCheckTimerHandle;
	FTimerHandle SpawnTimerHandle;
	FTimerHandle AttackSelectTimerHandle;

private:
	void RequestSpawnGuardian(int RequestCount);
	void ProcessSpawnQueue();
	void SpawnGuardian(const FTransform& InSpawnTransform);
	TArray<FTransform> FindSafeSpawnPoints(int RequestCount = 0) const;
	
	// 후보 Guardian들에 대해 점수를 매겨 가장 적합한 공격자를 반환
	AGuardian* EvaluateBestAttacker() const;
	
private:
	bool bIsEnabled{false};

	// 페이즈1에 죽여야 하는 Guardian 수
	UPROPERTY()
	int32 Phase1GuardianCount{8};
	
	UPROPERTY()
	int32 MaxAliveCount{5};
	
	UPROPERTY()
	int32 MaxSpawnCountPerRequest{2};
	
	UPROPERTY()
	float SpawnCheckInterval{1.5f};

	UPROPERTY()
	float SpawnInterval{0.2f};

	// 공격자 선택 주기 (초)
	UPROPERTY()
	float AttackSelectInterval{10.f};
	
	TSubclassOf<AGuardian> GuardianClass;;
	
	// Reset을 해줘야 하는 데이터
	int32 PendingSpawnCount{0};

	TWeakObjectPtr<AGuardian> CombatingGuardian;

	TArray<TWeakObjectPtr<AGuardian>> AliveGuardians;
	
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<AEnemySpawnPoint>> SpawnPoints;
	// --------------------------------
	
	SpawnPointGenerator SpawnPointsGenerator{};
};
