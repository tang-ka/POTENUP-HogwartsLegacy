// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GuardianManager.h"

#include "HogwartsLegacy.h"
#include "Enemy/EnemyFSMComponent.h"
#include "Enemy/Guardian.h"
#include "Enemy/EnemySpawnPoint.h"
#include "Enemy/GuardianSettings.h"
#include "UI/ControllerHUD.h"

void SpawnPointGenerator::Generate(UWorld* World, TArray<TObjectPtr<AEnemySpawnPoint>>& OutSpawnPoints) const
{
	const float AngleStep = 360.f / SpawnPointCount;

	for (int i = 0; i < SpawnPointCount; ++i)
	{
		float Angle = i * AngleStep;
		const float x = FMath::Cos(FMath::DegreesToRadians(Angle)) * SpawnRadius;
		const float y = FMath::Sin(FMath::DegreesToRadians(Angle)) * SpawnRadius;
		constexpr float z = 50.f;
		FVector SpawnLocation = FVector(x, y, z);

		// SpawnLocation과 CenterLocation의 Z축을 무시
		FVector FlatSpawnLocation = FVector(SpawnLocation.X, SpawnLocation.Y, CenterLocation.Z);
		FVector Direction = CenterLocation - FlatSpawnLocation;
		FRotator LookAtRotation = Direction.Rotation();

		FTransform SpawnTransform(LookAtRotation, SpawnLocation);

		FActorSpawnParameters SpawnParams;
		// 스폰 시 충돌이 있어도 위치를 조금 조정해서 무조건 스폰되게 보장하는 옵션
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AEnemySpawnPoint* NewSpawnPoint = World->SpawnActor<AEnemySpawnPoint>(
			AEnemySpawnPoint::StaticClass(), SpawnTransform, SpawnParams);
		if (NewSpawnPoint)
		{
			OutSpawnPoints.Add(NewSpawnPoint);
			NewSpawnPoint->SetFolderPath(FName("SpawnPoints"));
		}
	}
}

void UGuardianManager::Enable()
{
	bIsEnabled = true;

	// 설정에서 GuardianClass 로드
	const UGuardianSettings* Settings = GetDefault<UGuardianSettings>();
	if (Settings && !Settings->GuardianClass.IsNull())
	{
		// 1. 이미 로드되어 있는지 확인 (메모리 효율)
		GuardianClass = Settings->GuardianClass.Get();

		// 2. 로드되어 있지 않다면 동기 로드 시도
		if (!GuardianClass)
		{
			GuardianClass = Settings->GuardianClass.LoadSynchronous();
		}
	}

	if (!GuardianClass)
	{
		UE_LOG(LogTemp, Error, TEXT("GuardianManager: Failed to load Guardian Class from Settings!"));
		// 하드코딩된 Fallback보다는 에러 로그를 남기고 시스템을 중단하는 것이 디버깅에 유리합니다.
		return;
	}

	if (UWorld* World = GetWorld())
	{
		SpawnPointsGenerator.Generate(World, SpawnPoints);
	}

	// 타이머 시작
	StartCheckTick();

	// 공격자 선택 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(AttackSelectTimerHandle,
	                                       this, &UGuardianManager::SelectAndCommandAttack,
	                                       AttackSelectInterval, true);
}

void UGuardianManager::Disable()
{
	bIsEnabled = false;

	// Reset
	GetWorld()->GetTimerManager().ClearTimer(SpawnCheckTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AttackSelectTimerHandle);
	PendingSpawnCount = 0;
	CombatingGuardian.Reset();
	AliveGuardians.Empty();
	SpawnPoints.Empty();
}

// ============================================================
// 공격 독점 관리 — Manager 주도
// ============================================================

void UGuardianManager::SelectAndCommandAttack()
{
	// 이미 공격 중인 Guardian이 살아있으면 새로 지시하지 않는다
	if (CombatingGuardian.IsValid())
	{
		PRINT_LOG(TEXT("[Manager] [%s] is already attacking. Skipping selection."), *CombatingGuardian->GetName());
		return;
	}

	// 가장 적합한 공격자를 선택
	AGuardian* BestAttacker = EvaluateBestAttacker();
	if (!BestAttacker)
	{
		PRINT_LOG(TEXT("[Manager] No valid attackers found. Skipping attack command."));
		return;
	}

	// 공격 지시
	CombatingGuardian = BestAttacker;
	CombatingGuardian->GetFSMComponent()->RequestAttack();
	PRINT_LOG(TEXT("[Manager] Selected [%s] to attack."), *BestAttacker->GetName());
}

void UGuardianManager::OnGuardianFinishedAttack()
{
	if (CombatingGuardian.IsValid())
	{
		PRINT_LOG(TEXT("[Manager] [%s] finished attack. Releasing combat slot."), *CombatingGuardian->GetName());
	}
	CombatingGuardian.Reset();
}

AGuardian* UGuardianManager::EvaluateBestAttacker() const
{
	AGuardian* BestGuardian = nullptr;
	float BestScore = -1.f;

	for (const TWeakObjectPtr<AGuardian>& WeakGuardian : AliveGuardians)
	{
		if (!WeakGuardian.IsValid()) continue;

		AGuardian* Guardian = WeakGuardian.Get();

		// 공격 가능 상태가 아니면 스킵
		if (!Guardian->IsAttackReady()) continue;

		const float Distance = Guardian->GetDistanceToTarget();
		const float Angle = Guardian->GetAngleToTarget();

		// 점수 산출: 거리가 가까울수록, 시야각이 좁을수록 (정면) 높은 점수
		// 거리 점수: 0~1 (가까울수록 1)
		const float MaxScoreDistance = 3000.f;
		const float DistScore = FMath::Clamp(1.f - (Distance / MaxScoreDistance), 0.f, 1.f);

		// 각도 점수: 0~1 (정면일수록 1)
		const float AngleScore = FMath::Clamp(1.f - (Angle / 180.f), 0.f, 1.f);

		// 가중합 (거리 70%, 각도 30%)
		const float Score = DistScore * 0.3f + AngleScore * 0.7f;

		if (Score > BestScore)
		{
			BestScore = Score;
			BestGuardian = Guardian;
		}
	}

	return BestGuardian;
}

void UGuardianManager::GameClear()
{
	Disable();
	
	GetWorld()->GetTimerManager().SetTimer(GameOverTimerHandle, [this]()
	{
		// HUD를 불러오고 싶다.
		auto* HUD = Cast<AControllerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
		HUD->Gameover();
	}, 
	GameOverDelay, false);
}

void UGuardianManager::StartCheckTick()
{
	// 타이머 등록 (CheckTick이 일정 간격으로 호출되도록 설정)
	GetWorld()->GetTimerManager().SetTimer(SpawnCheckTimerHandle, this, &UGuardianManager::CheckTick,
	                                       SpawnCheckInterval, true);
}

void UGuardianManager::CheckTick()
{
	// 살아 있는 Guardian 수가 MaxAliceCount보다 적으면 Spawn 요청
	int MaxCount = FMath::Min(MaxAliveCount, Phase1GuardianCount);
	if (GetCurrentAliveCount() < MaxCount)
	{
		RequestSpawnGuardian(MaxCount - GetCurrentAliveCount());
	}
}

void UGuardianManager::RequestSpawnGuardian(int RequestCount)
{
	// 한번에 스폰 가능한 Guardian 만큼만 스폰 요청
	int RequestOnce = FMath::Min(RequestCount, MaxSpawnCountPerRequest);

	if (RequestOnce <= 0)
	{
		return;
	}

	PendingSpawnCount += RequestOnce; // 누적

	// 큐가 처리 되는 동안 중복 요청 방지 위해 타이머 해제
	GetWorld()->GetTimerManager().ClearTimer(SpawnCheckTimerHandle);

	// 스폰 큐 타이머가 돌고 있지 않다면 즉시 처리 시작
	if (!GetWorld()->GetTimerManager().IsTimerActive(SpawnTimerHandle))
	{
		ProcessSpawnQueue();
	}
}

void UGuardianManager::ProcessSpawnQueue()
{
	if (!bIsEnabled)
	{
		return;
	}

	if (PendingSpawnCount <= 0)
	{
		StartCheckTick();
		return;
	}

	TArray<FTransform> SafeSpawnPoints = FindSafeSpawnPoints(1);
	if (SafeSpawnPoints.Num() > 0)
	{
		SpawnGuardian(SafeSpawnPoints[0]);
		PendingSpawnCount--;
	}
	else
	{
		PRINT_LOG(TEXT("No safe spawn points available. Will retry next tick."));
	}

	// 아직 처리할게 있는지 확인
	if (PendingSpawnCount > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &UGuardianManager::ProcessSpawnQueue,
		                                       SpawnInterval, false);
	}
	else
	{
		StartCheckTick();
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
	if (NewGuardian)
	{
		AliveGuardians.Add(NewGuardian);

		// 공격 종료 콜백: Guardian의 공격이 끝나면 Manager에게 알림
		NewGuardian->GetFSMComponent()->OnAttackFinished.AddUObject(this, &UGuardianManager::OnGuardianFinishedAttack);

		NewGuardian->GetFSMComponent()->OnEnemyDead.AddLambda([this, NewGuardian]()
		{
			AliveGuardians.Remove(NewGuardian);
			Phase1GuardianCount--;

			if (Phase1GuardianCount <= 0)
			{
				GameClear();
			}
		});
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
			SafeSpawnTransforms.Add(SpawnPoint->GetSafeSpawnPoint());
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
