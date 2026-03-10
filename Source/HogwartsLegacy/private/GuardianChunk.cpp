// Fill out your copyright notice in the Description page of Project Settings.

#include "GuardianChunk.h"

#include "Async/Async.h"
#include "ProfilingDebugging/CPUProfilerTrace.h"
#include "HogwartsLegacy.h"
#include "ProceduralMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "SkeletalRenderPublic.h"

AGuardianChunk::AGuardianChunk()
{
	PrimaryActorTick.bCanEverTick = true;
	
	ProcMeshComp = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMeshComp"));
	SetRootComponent(ProcMeshComp);
}

void AGuardianChunk::BeginPlay()
{
	Super::BeginPlay();
}

void AGuardianChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ── 비동기 메시 빌드 완료 → 프레임 카운터 시작 ──
	if (bMeshBuildComplete)
	{
		bMeshBuildComplete = false;

		// PendingApplyFrames가 아직 설정되지 않았다면 0으로 (즉시 적용)
		if (PendingApplyFrames < 0)
		{
			PendingApplyFrames = 0;
		}
	}

	// ── 메시 적용 프레임 카운터 ──
	if (PendingApplyFrames >= 0 && PendingBuildResult.IsValid())
	{
		if (PendingApplyFrames == 0)
		{
			ApplyMeshBuildResult(*PendingBuildResult);
			PendingBuildResult.Reset();
			PendingApplyFrames = -1;
		}
		else
		{
			--PendingApplyFrames;
		}
	}

	// ── 프레임 카운터 기반 순차 물리 활성화 ──
	// 메시가 아직 적용되지 않았으면(CachedVertices가 비어있으면) 물리 활성화를 건너뜀
	if (PendingPhysicsFrames >= 0 && CachedVertices.Num() > 0)
	{
		if (PendingPhysicsFrames == 0)
		{
			ActivatePhysicsWithExplosion(PendingExplosionCenter, PendingExplosionStrength, PendingExplosionRadius);
			PendingPhysicsFrames = -1;

			// 물리 활성화 완료 → Tick 불필요
			SetActorTickEnabled(false);
		}
		else
		{
			--PendingPhysicsFrames;
		}
	}
}

void AGuardianChunk::InitializeChunk(USkeletalMeshComponent* InSourceMeshComp, const TSet<int32>& InVertexIndices, UMaterialInterface* InCapMaterial, int32 ApplyDelayFrames)
{
	if (!InSourceMeshComp)
	{
		PRINT_LOG(TEXT("Source MeshComponent is NULL"));
		return;
	}

	if (InVertexIndices.Num() == 0)
	{
		PRINT_LOG(TEXT("No vertex indices assigned to this chunk"));
		return;
	}
	
	CapMaterial = InCapMaterial;

	// 메시 적용 지연 프레임 설정
	PendingApplyFrames = FMath::Max(ApplyDelayFrames, 0);

	// 소스 머티리얼 캐시 (비동기 완료 후 게임 스레드에서 적용하기 위해 미리 복사)
	CachedMaterials.Reset();
	for (int32 i = 0; i < InSourceMeshComp->GetNumMaterials(); ++i)
	{
		CachedMaterials.Add(InSourceMeshComp->GetMaterial(i));
	}

	// ── 1단계: 게임 스레드에서 스냅샷 수집 ──
	TSharedPtr<FChunkMeshSnapshot> Snapshot = MakeShared<FChunkMeshSnapshot>();
	if (!CollectMeshSnapshot(InSourceMeshComp, InVertexIndices, *Snapshot))
	{
		PRINT_LOG(TEXT("Failed to collect mesh snapshot"));
		return;
	}

	SetupChunkSettings(InSourceMeshComp);

	// ── 2단계: 워커 스레드에서 메시 빌드 ──
	PendingBuildResult = MakeShared<FChunkMeshBuildResult>();
	TSharedPtr<FChunkMeshBuildResult> ResultPtr = PendingBuildResult;
	FThreadSafeBool* CompleteFlagPtr = &bMeshBuildComplete;

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [Snapshot, ResultPtr, CompleteFlagPtr]()
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(AGuardianChunk::BuildMeshDataAsync);
		BuildMeshDataAsync(Snapshot, ResultPtr);
		*CompleteFlagPtr = true;
	});
}

// ===========================================================================
// 1단계: 게임 스레드 — 렌더 데이터 스냅샷 수집
// ===========================================================================
bool AGuardianChunk::CollectMeshSnapshot(USkeletalMeshComponent* InSourceMeshComp, const TSet<int32>& InVertexIndices, FChunkMeshSnapshot& OutSnapshot)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AGuardianChunk::CollectMeshSnapshot);

	constexpr int32 LODIndex = 0;
	const FSkeletalMeshRenderData* RenderData = InSourceMeshComp->GetSkeletalMeshRenderData();
	if (!RenderData || !RenderData->LODRenderData.IsValidIndex(LODIndex))
	{
		PRINT_LOG(TEXT("SkeletalMeshRenderData is invalid"));
		return false;
	}

	const FSkeletalMeshLODRenderData& LODData = RenderData->LODRenderData[LODIndex];
	const FStaticMeshVertexBuffer& StaticMeshVertexBuffer = LODData.StaticVertexBuffers.StaticMeshVertexBuffer;

	// CPU 스키닝 결과 가져오기
	TArray<FFinalSkinVertex> SkinnedVertices;
	InSourceMeshComp->GetCPUSkinnedVertices(SkinnedVertices, LODIndex);

	const int32 TotalVertices = SkinnedVertices.Num();
	if (TotalVertices <= 0)
	{
		PRINT_LOG(TEXT("Failed to read CPU skinned vertices"));
		return false;
	}

	if (TotalVertices != static_cast<int32>(LODData.GetNumVertices()))
	{
		PRINT_LOG(TEXT("Skinned vertex count mismatch"));
		return false;
	}

	// 스키닝 결과를 불변 배열로 복사
	OutSnapshot.SkinnedPositions.SetNumUninitialized(TotalVertices);
	OutSnapshot.SkinnedTangentX.SetNumUninitialized(TotalVertices);
	OutSnapshot.SkinnedTangentZ.SetNumUninitialized(TotalVertices);
	OutSnapshot.UVs.SetNumUninitialized(TotalVertices);

	for (int32 i = 0; i < TotalVertices; ++i)
	{
		const FFinalSkinVertex& SV = SkinnedVertices[i];
		OutSnapshot.SkinnedPositions[i] = SV.Position;
		OutSnapshot.SkinnedTangentX[i] = SV.TangentX.ToFVector3f();
		OutSnapshot.SkinnedTangentZ[i] = SV.TangentZ.ToFVector3f();
		OutSnapshot.UVs[i] = StaticMeshVertexBuffer.GetVertexUV(i, 0);
	}

	// 인덱스 버퍼 복사
	FMultiSizeIndexContainerData IndicesData;
	LODData.MultiSizeIndexContainer.GetIndexBuffer(IndicesData.Indices);
	OutSnapshot.IndexBuffer = MoveTemp(IndicesData.Indices);

	// 섹션 정보 복사
	OutSnapshot.Sections.Reserve(LODData.RenderSections.Num());
	for (const FSkelMeshRenderSection& Section : LODData.RenderSections)
	{
		FChunkMeshSnapshot::FSectionInfo Info;
		Info.BaseIndex = Section.BaseIndex;
		Info.NumTriangles = Section.NumTriangles;
		Info.MaterialIndex = Section.MaterialIndex;
		OutSnapshot.Sections.Add(Info);
	}

	// 버텍스 인덱스 정렬 복사
	OutSnapshot.SortedVertexIndices = InVertexIndices.Array();
	OutSnapshot.SortedVertexIndices.Sort();

	return true;
}

// ===========================================================================
// 2단계: 워커 스레드 — 순수 데이터 연산 (UObject 접근 없음)
// ===========================================================================
void AGuardianChunk::BuildMeshDataAsync(TSharedPtr<FChunkMeshSnapshot> Snapshot, TSharedPtr<FChunkMeshBuildResult> OutResult)
{
	const FChunkMeshSnapshot& Data = *Snapshot;
	FChunkMeshBuildResult& Result = *OutResult;

	const int32 NumVerts = Data.SortedVertexIndices.Num();

	// 버텍스 리매핑 + 어트리뷰트 배열 구축
	TMap<int32, int32> OldToNewIndex;
	OldToNewIndex.Reserve(NumVerts);

	Result.Vertices.Reserve(NumVerts);
	Result.Normals.Reserve(NumVerts);
	Result.UV0.Reserve(NumVerts);
	Result.VertexColors.Reserve(NumVerts);
	Result.Tangents.Reserve(NumVerts);

	for (const int32 OrigIdx : Data.SortedVertexIndices)
	{
		const int32 NewIdx = Result.Vertices.Num();
		OldToNewIndex.Add(OrigIdx, NewIdx);

		Result.Vertices.Add(FVector(Data.SkinnedPositions[OrigIdx]));
		Result.Normals.Add(FVector(Data.SkinnedTangentZ[OrigIdx]));
		Result.UV0.Add(FVector2D(Data.UVs[OrigIdx]));
		Result.VertexColors.Add(FColor::White);
		Result.Tangents.Add(FProcMeshTangent(FVector(Data.SkinnedTangentX[OrigIdx]), false));
	}

	// 섹션별 삼각형 필터링
	TArray<int32> AllChunkTriangles;

	for (const FChunkMeshSnapshot::FSectionInfo& SectionInfo : Data.Sections)
	{
		TArray<int32> Triangles;

		for (uint32 TriIdx = 0; TriIdx < SectionInfo.NumTriangles; ++TriIdx)
		{
			const uint32 BaseIdx = SectionInfo.BaseIndex + TriIdx * 3;
			const int32 I0 = Data.IndexBuffer[BaseIdx + 0];
			const int32 I1 = Data.IndexBuffer[BaseIdx + 1];
			const int32 I2 = Data.IndexBuffer[BaseIdx + 2];

			const int32* New0 = OldToNewIndex.Find(I0);
			const int32* New1 = OldToNewIndex.Find(I1);
			const int32* New2 = OldToNewIndex.Find(I2);

			if (New0 && New1 && New2)
			{
				Triangles.Add(*New0);
				Triangles.Add(*New1);
				Triangles.Add(*New2);
			}
		}

		if (Triangles.Num() == 0)
		{
			continue;
		}

		AllChunkTriangles.Append(Triangles);

		FChunkMeshBuildResult::FSectionData SectionData;
		SectionData.Triangles = MoveTemp(Triangles);
		SectionData.MaterialIndex = SectionInfo.MaterialIndex;
		Result.Sections.Add(MoveTemp(SectionData));
	}

	// 캡 메시 계산
	BuildCapSections(Result.Vertices, AllChunkTriangles, Result.CapSections);
}

// ===========================================================================
// 캡 섹션 생성 (순수 함수, 워커 스레드 안전)
// ===========================================================================
void AGuardianChunk::BuildCapSections(const TArray<FVector>& InVertices, const TArray<int32>& InAllTriangles,
                                      TArray<FChunkMeshBuildResult::FCapSectionData>& OutCapSections)
{
	if (InVertices.Num() == 0 || InAllTriangles.Num() == 0)
	{
		return;
	}

	// 1. 에지 빈도수 계산 및 원본 폴리곤의 방향성 유지
	TMap<uint64, int32> EdgeCount;
	TArray<TPair<int32, int32>> DirectedEdges;

	for (int32 i = 0; i < InAllTriangles.Num(); i += 3)
	{
		for (int32 j = 0; j < 3; ++j)
		{
			int32 V0 = InAllTriangles[i + j];
			int32 V1 = InAllTriangles[i + (j + 1) % 3];

			uint64 EdgeKey = (static_cast<uint64>(FMath::Min(V0, V1)) << 32) | static_cast<uint64>(FMath::Max(V0, V1));

			if (EdgeCount.Contains(EdgeKey))
			{
				EdgeCount[EdgeKey]++;
			}
			else
			{
				EdgeCount.Add(EdgeKey, 1);
			}

			DirectedEdges.Add(TPair<int32, int32>(V0, V1));
		}
	}

	// 2. 외곽선(빈도수가 1인 에지)만 필터링
	TMap<int32, int32> NextVertexMap;
	for (const auto& Edge : DirectedEdges)
	{
		uint64 EdgeKey = (static_cast<uint64>(FMath::Min(Edge.Key, Edge.Value)) << 32) | static_cast<uint64>(FMath::Max(Edge.Key, Edge.Value));

		if (EdgeCount[EdgeKey] == 1)
		{
			NextVertexMap.Add(Edge.Key, Edge.Value);
		}
	}

	// 3. 그래프 탐색으로 루프 추출
	TSet<int32> VisitedVertices;

	for (const auto& Pair : NextVertexMap)
	{
		int32 StartVertex = Pair.Key;

		if (!VisitedVertices.Contains(StartVertex))
		{
			TArray<int32> CurrentLoop;
			int32 CurrentVertex = StartVertex;

			while (!VisitedVertices.Contains(CurrentVertex) && NextVertexMap.Contains(CurrentVertex))
			{
				VisitedVertices.Add(CurrentVertex);
				CurrentLoop.Add(CurrentVertex);
				CurrentVertex = NextVertexMap[CurrentVertex];
			}

			if (CurrentLoop.Num() >= 3)
			{
				// 4. 루프 면적 계산 (Shoelace 방식) — 작은 구멍은 캡 생성 스킵
				FVector Centroid = FVector::ZeroVector;
				for (int32 VertexIndex : CurrentLoop)
				{
					Centroid += InVertices[VertexIndex];
				}
				Centroid /= static_cast<float>(CurrentLoop.Num());

				// 삼각형 팬 면적 합산
				float LoopArea = 0.f;
				for (int32 i = 0; i < CurrentLoop.Num(); ++i)
				{
					const FVector& V0 = InVertices[CurrentLoop[i]];
					const FVector& V1 = InVertices[CurrentLoop[(i + 1) % CurrentLoop.Num()]];
					LoopArea += FVector::CrossProduct(V0 - Centroid, V1 - Centroid).Size() * 0.5f;
				}

				// 유의미한 크기 미만의 구멍은 무시 (단위: cm²)
				constexpr float MinCapArea = 10.f;
				if (LoopArea < MinCapArea)
				{
					continue;
				}

				// 5. 캡 섹션 데이터 생성
				FChunkMeshBuildResult::FCapSectionData CapData;

				int32 StartIndex = CapData.Vertices.Num();
				CapData.Vertices.Add(Centroid);
				CapData.UVs.Add(FVector2D(0.5f, 0.5f));
				CapData.Colors.Add(FColor::White);

				FVector CapNormal = FVector::UpVector;
				FProcMeshTangent CapTangent = FProcMeshTangent(FVector::ForwardVector, false);

				for (int32 i = 0; i < CurrentLoop.Num(); ++i)
				{
					int32 VIndex = CurrentLoop[i];
					CapData.Vertices.Add(InVertices[VIndex]);
					CapData.UVs.Add(FVector2D(0.0f, 0.0f));
					CapData.Colors.Add(FColor::White);

					int32 NextI = (i + 1) % CurrentLoop.Num();

					CapData.Triangles.Add(StartIndex);
					CapData.Triangles.Add(StartIndex + 1 + NextI);
					CapData.Triangles.Add(StartIndex + 1 + i);
				}

				for (int32 i = 0; i < CapData.Vertices.Num(); ++i)
				{
					CapData.Normals.Add(CapNormal);
					CapData.Tangents.Add(CapTangent);
				}

				OutCapSections.Add(MoveTemp(CapData));
			}
		}
	}
}

// ===========================================================================
// 3단계: 게임 스레드 — ProceduralMeshComponent에 결과 적용
// ===========================================================================
void AGuardianChunk::ApplyMeshBuildResult(const FChunkMeshBuildResult& Result)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AGuardianChunk::ApplyMeshBuildResult);

	if (!ProcMeshComp)
	{
		return;
	}

	ProcMeshComp->ClearAllMeshSections();

	int32 SectionIndex = 0;

	// 메인 섹션 적용
	for (const FChunkMeshBuildResult::FSectionData& Section : Result.Sections)
	{
		ProcMeshComp->CreateMeshSection(SectionIndex, Result.Vertices, Section.Triangles,
		                                Result.Normals, Result.UV0, Result.VertexColors, Result.Tangents, true);

		if (CachedMaterials.IsValidIndex(Section.MaterialIndex))
		{
			ProcMeshComp->SetMaterial(SectionIndex, CachedMaterials[Section.MaterialIndex]);
		}
		++SectionIndex;
	}

	// 캡 섹션 적용
	// for (const FChunkMeshBuildResult::FCapSectionData& CapData : Result.CapSections)
	// {
	// 	ProcMeshComp->CreateMeshSection(SectionIndex, CapData.Vertices, CapData.Triangles,
	// 	                                CapData.Normals, CapData.UVs, CapData.Colors, CapData.Tangents, true);
	//
	// 	if (CapMaterial)
	// 	{
	// 		ProcMeshComp->SetMaterial(SectionIndex, CapMaterial);
	// 	}
	// 	++SectionIndex;
	// }

	// 물리 충돌체 생성용 버텍스 캐시
	CachedVertices = Result.Vertices;

	ProcMeshComp->SetVisibility(true, true);
}

void AGuardianChunk::EnableChunkPhysics()
{
	if (!ProcMeshComp)
	{
		return;
	}

	if (CachedVertices.Num() == 0)
	{
		PRINT_LOG(TEXT("No cached vertices for collision"));
		return;
	}

	// 1. 동적 물리 시뮬레이션을 위해 복합 충돌 사용 해제
	ProcMeshComp->bUseComplexAsSimpleCollision = false;

	// 2. 기존 충돌체 초기화 및 버텍스 기반 Convex Hull 충돌체 생성
	ProcMeshComp->ClearCollisionConvexMeshes();
	ProcMeshComp->AddCollisionConvexMesh(CachedVertices);

	// 3. 물리 연산을 위한 충돌 속성 재설정
	ProcMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProcMeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));

	// 4. 물리 및 중력 시뮬레이션 켜기
	ProcMeshComp->SetSimulatePhysics(true);
	ProcMeshComp->SetEnableGravity(true);

	// 5. 질량 설정 (기본값이 너무 크거나 작을 수 있으므로 적절한 값 지정)
	ProcMeshComp->SetMassOverrideInKg(NAME_None, 5.0f);
	
	// 6. 감속 설정 (너무 빨리 미끄러지지 않도록)
	ProcMeshComp->SetLinearDamping(0.5f);
	ProcMeshComp->SetAngularDamping(0.5f);
}

void AGuardianChunk::SchedulePhysicsActivation(const FVector& ExplosionCenter, float ExplosionStrength, float ExplosionRadius, int32 DelayFrames)
{
	PendingExplosionCenter = ExplosionCenter;
	PendingExplosionStrength = ExplosionStrength;
	PendingExplosionRadius = ExplosionRadius;
	PendingPhysicsFrames = FMath::Max(DelayFrames, 0);
}

void AGuardianChunk::ActivatePhysicsWithExplosion(const FVector& ExplosionCenter, float ExplosionStrength, float ExplosionRadius)
{
	// 물리 시뮬레이션 활성화
	EnableChunkPhysics();

	if (!ProcMeshComp || !ProcMeshComp->IsSimulatingPhysics())
	{
		PRINT_LOG(TEXT("Physics simulation failed to activate"));
		return;
	}

	// Chunk 중심 계산
	const FVector ChunkCenter = CalculateChunkCenter();

	// 폭발 중심에서 Chunk 중심으로의 방향 벡터
	FVector ExplosionDir = ChunkCenter - ExplosionCenter;
	const float Distance = ExplosionDir.Size();

	if (Distance < KINDA_SMALL_NUMBER)
	{
		// 너무 가까우면 임의 방향으로 날려보냄
		ExplosionDir = FMath::VRand();
	}
	else
	{
		ExplosionDir.Normalize();
	}

	// 거리에 따른 감쇠 (반경 밖이면 최소 힘 적용)
	const float DistanceFactor = FMath::Clamp(1.0f - (Distance / ExplosionRadius), 0.1f, 1.0f);

	// 약간의 위쪽 방향 추가 (터져서 공중으로 튀어오르는 느낌)
	const FVector UpwardBias = FVector(0.0f, 0.0f, 0.3f);
	const FVector FinalDir = (ExplosionDir + UpwardBias).GetSafeNormal();

	// Impulse 적용
	const FVector Impulse = FinalDir * ExplosionStrength * DistanceFactor;
	ProcMeshComp->AddImpulse(Impulse, NAME_None, true);

	// 회전 Impulse 추가 (파편이 회전하면서 날아가는 느낌)
	const FVector RandomTorque = FMath::VRand() * ExplosionStrength * DistanceFactor * 0.3f;
	ProcMeshComp->AddAngularImpulseInDegrees(RandomTorque, NAME_None, true);
}

FVector AGuardianChunk::CalculateChunkCenter() const
{
	if (CachedVertices.Num() == 0)
	{
		return GetActorLocation();
	}

	FVector Center = FVector::ZeroVector;
	for (const FVector& Vert : CachedVertices)
	{
		Center += Vert;
	}
	Center /= static_cast<float>(CachedVertices.Num());

	// 로컬 좌표 → 월드 좌표 변환
	return GetActorTransform().TransformPosition(Center);
}

void AGuardianChunk::SetupChunkSettings(USkeletalMeshComponent* InSourceMeshComp) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AGuardianChunk::SetupChunkSettings);
	ProcMeshComp->SetRelativeTransform(InSourceMeshComp->GetComponentTransform());
	ProcMeshComp->SetCollisionEnabled(InSourceMeshComp->GetCollisionEnabled());
	ProcMeshComp->SetCollisionProfileName(InSourceMeshComp->GetCollisionProfileName());
	ProcMeshComp->SetCollisionObjectType(InSourceMeshComp->GetCollisionObjectType());
	ProcMeshComp->SetCanEverAffectNavigation(false);
	ProcMeshComp->SetCastShadow(InSourceMeshComp->CastShadow);
	ProcMeshComp->bUseComplexAsSimpleCollision = true;
	ProcMeshComp->SetVisibility(true, true);
}
