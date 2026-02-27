// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianManager.h"

#include "HogwartsLegacy.h"
#include "Enemy/Guardian.h"
#include "Enemy/EnemySpawnPoint.h"

void SpawnPointGenerator::Generate(UWorld* World, TArray<TObjectPtr<AEnemySpawnPoint>>& OutSpawnPoints) const
{
	const float AngleStep = 360.f / SpawnPointCount;
	
	for (int i = 0; i < SpawnPointCount; ++i)
	{
		float Angle = i * AngleStep;
		const float x = FMath::Cos(FMath::DegreesToRadians(Angle)) * SpawnRadius;
		const float y = FMath::Sin(FMath::DegreesToRadians(Angle)) * SpawnRadius;
		const float z = 100.f;
		FVector SpawnLocation = FVector(x, y, z);
		
		FVector Direction = CenterLocation - SpawnLocation;
		FRotator LookAtRotation = Direction.Rotation();
		
		FTransform SpawnTransform(LookAtRotation, SpawnLocation);
		
		FActorSpawnParameters SpawnParams;
		// 스폰 시 충돌이 있어도 위치를 조금 조정해서 무조건 스폰되게 보장하는 옵션
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		AEnemySpawnPoint* NewSpawnPoint = World->SpawnActor<AEnemySpawnPoint>(AEnemySpawnPoint::StaticClass(), SpawnTransform, SpawnParams);
		if (NewSpawnPoint)
		{
			OutSpawnPoints.Add(NewSpawnPoint);
			NewSpawnPoint->SetFolderPath(FName("SpawnPoints"));
		}
	}
}

void UGuardianManager::RequestSpawnGuardian()
{
	// 필요한 Guardian 수 계산 (예: 최대 5마리, 현재 3마리 -> 2마리 필요)
	int32 NeededCount = MaxAliveCount - AliveGuardians.Num();
	if (NeededCount <= 0)
	{
		PRINT_LOG(TEXT("Maximum alive guardians reached. No need to spawn more."));
		return;
	}
	
	// 한번에 최대 가능한 Guardian 만큼만 스폰 요청
	NeededCount = FMath::Min(NeededCount, MaxSpawnCountPerRequest);
	TArray<FTransform> SafeSpawnPoints = FindSafeSpawnPoints(NeededCount);
	for (const FTransform& SpawnTransform : SafeSpawnPoints)
	{
		SpawnGuardian(SpawnTransform);
	}
}

void UGuardianManager::SpawnGuardian(const FTransform& InSpawnTransform)
{
	if (!bIsEnabled)
	{
		PRINT_LOG(TEXT("Guardian Manager is disabled. Cannot spawn guardian."));
		return;
	}
	
	AGuardian* NewGuardian = GetWorld()->SpawnActor<AGuardian>(GuardianClass, InSpawnTransform);
	
	// AliveGuardians 추가
	if (NewGuardian != nullptr)
	{
		AliveGuardians.Add(NewGuardian);
	}
	else
	{
		PRINT_LOG(TEXT("Failed to spawn Guardian actor."));
	}
}

TArray<FTransform> UGuardianManager::FindSafeSpawnPoints(int RequestCount) const
{
	if (SpawnPoints.Num() == 0)
	{
		PRINT_LOG(TEXT("No spawn points defined for Guardian Manager."));
		return TArray<FTransform>();
	}
	
	TArray<FTransform> SafeSpawnTransforms{};
	
	for (auto SpawnPoint : SpawnPoints)
	{
		if (SpawnPoint->IsSafeToSpawn())
		{
			SafeSpawnTransforms.Add(SpawnPoint->GetActorTransform());
		}
	}
	// SafeSpawnTransforms에서 RequestCount 개수만큼 랜덤으로 선택하여 반환
	
	if (RequestCount > 0)
	{
		TArray<FTransform> SelectedSpawnTransforms{};
		int32 AvailableCount = SafeSpawnTransforms.Num();
		
		// 1. SafeSPawnTransform의 개수가 RequestCount보다 작거나 같으면 SafeSpawnTransforms 전체를 반환
		if (AvailableCount <= RequestCount)
		{
			return SafeSpawnTransforms;
		}
		
		// 2. SafeSpawnTransforms의 개수가 RequestCount보다 크면 RequestCount 개수만큼 랜덤으로 선택하여 반환
		TArray<int32> Indices;
		while (Indices.Num() < RequestCount)
		{
			int32 RandomIndex = FMath::RandRange(0, AvailableCount - 1);
			if (!Indices.Contains(RandomIndex))
			{
				Indices.Add(RandomIndex);
				SelectedSpawnTransforms.Add(SafeSpawnTransforms[RandomIndex]);
			}
		}
		
		return SelectedSpawnTransforms;
	}
	else
	{
		return SafeSpawnTransforms;
	}
}
