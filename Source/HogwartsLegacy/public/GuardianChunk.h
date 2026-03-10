// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "GuardianChunk.generated.h"

class UProceduralMeshComponent;
class USkeletalMeshComponent;

// ---------------------------------------------------------------------------
// 게임 스레드에서 수집한 불변 스냅샷 (워커 스레드에 전달)
// ---------------------------------------------------------------------------
struct FChunkMeshSnapshot
{
	/** CPU 스키닝 결과 (FFinalSkinVertex 대신 필요한 데이터만 추출) */
	TArray<FVector3f> SkinnedPositions;
	TArray<FVector3f> SkinnedTangentX;
	TArray<FVector3f> SkinnedTangentZ;

	/** UV 데이터 */
	TArray<FVector2f> UVs;

	/** 인덱스 버퍼 */
	TArray<uint32> IndexBuffer;

	/** 섹션 정보 */
	struct FSectionInfo
	{
		uint32 BaseIndex;
		uint32 NumTriangles;
		int32 MaterialIndex;
	};
	TArray<FSectionInfo> Sections;

	/** 이 Chunk가 담당하는 버텍스 인덱스 (정렬 완료) */
	TArray<int32> SortedVertexIndices;
};

// ---------------------------------------------------------------------------
// 워커 스레드에서 생성한 메시 빌드 결과 (게임 스레드에서 적용)
// ---------------------------------------------------------------------------
struct FChunkMeshBuildResult
{
	/** 공통 버텍스 데이터 */
	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	/** 섹션별 삼각형 + 머티리얼 인덱스 */
	struct FSectionData
	{
		TArray<int32> Triangles;
		int32 MaterialIndex;
	};
	TArray<FSectionData> Sections;

	/** 캡 섹션 데이터 */
	struct FCapSectionData
	{
		TArray<FVector> Vertices;
		TArray<int32> Triangles;
		TArray<FVector> Normals;
		TArray<FVector2D> UVs;
		TArray<FColor> Colors;
		TArray<FProcMeshTangent> Tangents;
	};
	TArray<FCapSectionData> CapSections;
};

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
	 * @param ApplyDelayFrames   비동기 빌드 완료 후 메시 적용까지 대기할 프레임 수 (프레임 분산용)
	 */
	void InitializeChunk(USkeletalMeshComponent* InSourceMeshComp, const TSet<int32>& InVertexIndices,
	                     UMaterialInterface* InCapMaterial, int32 ApplyDelayFrames = 0);

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
	// ── 1단계: 게임 스레드에서 렌더 데이터를 불변 구조체로 스냅샷 ──
	bool CollectMeshSnapshot(USkeletalMeshComponent* InSourceMeshComp, const TSet<int32>& InVertexIndices, FChunkMeshSnapshot& OutSnapshot);

	// ── 2단계: 워커 스레드에서 순수 데이터 연산 (리매핑, 삼각형 추출, 캡 계산) ──
	static void BuildMeshDataAsync(TSharedPtr<FChunkMeshSnapshot> Snapshot, TSharedPtr<FChunkMeshBuildResult> OutResult);

	// ── 3단계: 게임 스레드에서 ProceduralMeshComponent에 결과 적용 ──
	void ApplyMeshBuildResult(const FChunkMeshBuildResult& Result);

	// ── 캡 메시 데이터 생성 (순수 함수, 워커 스레드 안전) ──
	static void BuildCapSections(const TArray<FVector>& InVertices, const TArray<int32>& InAllTriangles,
	                             TArray<FChunkMeshBuildResult::FCapSectionData>& OutCapSections);

	void EnableChunkPhysics();
	void SetupChunkSettings(USkeletalMeshComponent* InSourceMeshComp) const;

	/** Chunk 메시의 중심점(월드 좌표)을 계산한다 */
	FVector CalculateChunkCenter() const;

private:
	UPROPERTY()
	TObjectPtr<UProceduralMeshComponent> ProcMeshComp{};
	
	UPROPERTY()
	TObjectPtr<UMaterialInterface> CapMaterial{};

	/** 소스 머티리얼 캐시 (비동기 완료 후 게임 스레드에서 적용) */
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInterface>> CachedMaterials;
	
	/** 물리 충돌체 생성에 사용할 버텍스 캐시 */
	TArray<FVector> CachedVertices;

	/** 비동기 메시 빌드 결과 (워커 스레드 완료 후 Tick에서 적용) */
	TSharedPtr<FChunkMeshBuildResult> PendingBuildResult;

	/** 비동기 메시 빌드 완료 플래그 (Atomic) */
	FThreadSafeBool bMeshBuildComplete{false};

	/** 메시 적용까지 남은 프레임 수 (-1이면 대기 없음) */
	int32 PendingApplyFrames{-1};

	/** 물리 활성화까지 남은 프레임 수 (-1이면 대기 없음) */
	int32 PendingPhysicsFrames{-1};

	/** 지연 물리 활성화용 파라미터 캐시 */
	FVector PendingExplosionCenter{FVector::ZeroVector};
	float PendingExplosionStrength{0.f};
	float PendingExplosionRadius{0.f};
};
