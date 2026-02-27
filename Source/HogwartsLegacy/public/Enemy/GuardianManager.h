// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GuardianManager.generated.h"

class AEnemySpawnPoint;
class AGuardian;
/**
 * 
 */
UCLASS()
class HOGWARTSLEGACY_API UGuardianManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	
	void Enable() { bIsEnabled = true; }
	void Disable() { bIsEnabled = false; }
	bool IsEnabled() const { return bIsEnabled; }
	
	void RequestSpawnGuardian();
	void SpawnGuardian(const FTransform& InSpawnTransform);
	
	// 스폰해야 할 가디언이 있는지 일정 시간 마다 검사한다.
	
	int32 GetCurrentAliveCount() const { return AliveGuardians.Num(); }
	
private:
	TArray<FTransform> FindSafeSpawnPoints(int RequestCount = 0) const;
	
private:
	bool bIsEnabled{false};
	
	UPROPERTY(EditAnywhere)
	int32 MaxAliveCount{5};
	
	UPROPERTY(EditAnywhere)
	int32 MaxSpawnCountPerRequest{2};
	
	UPROPERTY(EditAnywhere)
	float SpawnCheckInterval{1.5f};

	UPROPERTY(EditAnywhere)
	float SpawnInterval{0.2f};

	TSubclassOf<AGuardian> GuardianClass;;
	
	TWeakObjectPtr<AGuardian> CombatingGuardian;
	TArray<TWeakObjectPtr<AGuardian>> AliveGuardians;
	
	UPROPERTY(VisibleAnywhere)
	TArray<TWeakObjectPtr<AEnemySpawnPoint>> SpawnPoints;
};
