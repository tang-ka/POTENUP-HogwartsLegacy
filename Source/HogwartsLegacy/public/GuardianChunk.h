// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GuardianChunk.generated.h"

class UProceduralMeshComponent;
class USkeletalMeshComponent;

UCLASS()
class HOGWARTSLEGACY_API AGuardianChunk : public AActor
{
	GENERATED_BODY()

public:
	AGuardianChunk();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	/**
	 * 소스 스켈레탈 메시에서, InVertexIndices에 해당하는 버텍스만으로 ProceduralMesh를 생성한다.
	 * @param InSourceMeshComp   소스 스켈레탈 메시 컴포넌트
	 * @param InVertexIndices    이 Chunk가 담당하는 버텍스 인덱스 집합
	 * @param InCapMaterial
	 */
	void InitializeChunk(USkeletalMeshComponent* InSourceMeshComp, const TSet<int32>& InVertexIndices,
	                     UMaterialInterface* InCapMaterial);

	/**
	 * 물리 시뮬레이션을 활성화하고 폭발 효과로 Chunk를 날려보낸다.
	 * @param ExplosionCenter   폭발 중심 (월드 좌표)
	 * @param ExplosionStrength 폭발 힘의 세기
	 * @param ExplosionRadius   폭발 반경
	 */
	void ActivatePhysicsWithExplosion(const FVector& ExplosionCenter, float ExplosionStrength = 100000.f, float ExplosionRadius = 500.f);

	/**
	 * 폭발 파라미터를 저장해두고, DelayFrames 프레임 후에 물리를 활성화한다.
	 * 각 Chunk에 서로 다른 DelayFrames를 부여하면 프레임별로 순차 활성화된다.
	 * Guardian이 Destroy되어도 Chunk는 독립 Actor이므로 자체 Tick에서 안전하게 처리된다.
	 * @param DelayFrames       물리 활성화까지 대기할 프레임 수 (0이면 다음 Tick에서 즉시)
	 */
	void SchedulePhysicsActivation(const FVector& ExplosionCenter, float ExplosionStrength, float ExplosionRadius, int32 DelayFrames = 0);

	UProceduralMeshComponent* GetProceduralMeshComponent() const { return ProcMeshComp; }

private:
	void GenerateProcMeshFromSkeletalMesh(USkeletalMeshComponent* InSourceMeshComp, const TSet<int32>& InVertexIndices);
	void GenerateProcMeshCapSection(const TArray<FVector>& CapVertices, const TArray<int32>& CapTriangles, int32 CapSectionIndex);
	void EnableChunkPhysics();
	void SetupChunkSettings(USkeletalMeshComponent* InSourceMeshComp) const;

	/** Chunk 메시의 중심점(월드 좌표)을 계산한다 */
	FVector CalculateChunkCenter() const;

private:
	UPROPERTY()
	TObjectPtr<UProceduralMeshComponent> ProcMeshComp{};
	
	UPROPERTY()
	TObjectPtr<UMaterialInterface> CapMaterial{};
	
	/** 물리 충돌체 생성에 사용할 버텍스 캐시 */
	TArray<FVector> CachedVertices;

	/** 물리 활성화까지 남은 프레임 수 (-1이면 대기 없음) */
	int32 PendingPhysicsFrames{-1};

	/** 지연 물리 활성화용 파라미터 캐시 */
	FVector PendingExplosionCenter{FVector::ZeroVector};
	float PendingExplosionStrength{0.f};
	float PendingExplosionRadius{0.f};
};
