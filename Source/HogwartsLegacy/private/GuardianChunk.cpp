// Fill out your copyright notice in the Description page of Project Settings.

#include "GuardianChunk.h"

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

	// 프레임 카운터 기반 순차 물리 활성화
	// 각 Chunk마다 다른 DelayFrames가 부여되어 프레임당 1개씩 Convex Hull 쿠킹이 수행됨
	if (PendingPhysicsFrames >= 0)
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

void AGuardianChunk::InitializeChunk(USkeletalMeshComponent* InSourceMeshComp, const TSet<int32>& InVertexIndices, UMaterialInterface* InCapMaterial)
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

	GenerateProcMeshFromSkeletalMesh(InSourceMeshComp, InVertexIndices);
	SetupChunkSettings(InSourceMeshComp);
}

void AGuardianChunk::GenerateProcMeshFromSkeletalMesh(USkeletalMeshComponent* InSourceMeshComp, const TSet<int32>& InVertexIndices)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AGuardianChunk::GenerateProcMeshFromSkeletalMesh);
	if (!ProcMeshComp || !InSourceMeshComp)
	{
		return;
	}

	constexpr int32 LODIndex = 0;
	const FSkeletalMeshRenderData* RenderData = InSourceMeshComp->GetSkeletalMeshRenderData();

	if (!RenderData || !RenderData->LODRenderData.IsValidIndex(LODIndex))
	{
		PRINT_LOG(TEXT("SkeletalMeshRenderData is invalid"));
		return;
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
		return;
	}

	if (TotalVertices != static_cast<int32>(LODData.GetNumVertices()))
	{
		PRINT_LOG(TEXT("Skinned vertex count mismatch"));
		return;
	}

	// 이 Chunk가 담당하는 버텍스만 추출하면서, 원본 인덱스 → 새 인덱스 리매핑 테이블 구축
	TMap<int32, int32> OldToNewIndex;
	OldToNewIndex.Reserve(InVertexIndices.Num());

	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;
	Vertices.Reserve(InVertexIndices.Num());
	Normals.Reserve(InVertexIndices.Num());
	UV0.Reserve(InVertexIndices.Num());
	VertexColors.Reserve(InVertexIndices.Num());
	Tangents.Reserve(InVertexIndices.Num());

	// InVertexIndices를 정렬된 배열로 변환 (결정적 순서)
	TArray<int32> SortedVertexIndices = InVertexIndices.Array();
	SortedVertexIndices.Sort();

	for (const int32 OrigIdx : SortedVertexIndices)
	{
		const int32 NewIdx = Vertices.Num();
		OldToNewIndex.Add(OrigIdx, NewIdx);

		const FFinalSkinVertex& SV = SkinnedVertices[OrigIdx];
		Vertices.Add(FVector(SV.Position));
		Normals.Add(FVector(SV.TangentZ.ToFVector3f()));
		UV0.Add(FVector2D(StaticMeshVertexBuffer.GetVertexUV(OrigIdx, 0)));
		VertexColors.Add(FColor::White);
		Tangents.Add(FProcMeshTangent(FVector(SV.TangentX.ToFVector3f()), false));
	}

	// 인덱스 버퍼
	FMultiSizeIndexContainerData IndicesData;
	LODData.MultiSizeIndexContainer.GetIndexBuffer(IndicesData.Indices);

	ProcMeshComp->ClearAllMeshSections();

	TArray<int32> AllChunkTriangles;
	int32 OutSectionIndex = 0;
	for (int32 SectionIndex = 0; SectionIndex < LODData.RenderSections.Num(); ++SectionIndex)
	{
		TArray<int32> Triangles;
		const FSkelMeshRenderSection& RenderSection = LODData.RenderSections[SectionIndex];

		// 이 섹션의 삼각형 중, 3개 꼭짓점 모두 우리 그룹에 속하는 것만 수집
		for (uint32 TriIdx = 0; TriIdx < RenderSection.NumTriangles; ++TriIdx)
		{
			const uint32 BaseIdx = RenderSection.BaseIndex + TriIdx * 3;
			const int32 I0 = IndicesData.Indices[BaseIdx + 0];
			const int32 I1 = IndicesData.Indices[BaseIdx + 1];
			const int32 I2 = IndicesData.Indices[BaseIdx + 2];

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
		
		ProcMeshComp->CreateMeshSection(OutSectionIndex, Vertices, Triangles, Normals, UV0, VertexColors,
		                                Tangents, true);

		if (InSourceMeshComp->GetNumMaterials() > static_cast<int32>(RenderSection.MaterialIndex))
		{
			ProcMeshComp->SetMaterial(OutSectionIndex, InSourceMeshComp->GetMaterial(RenderSection.MaterialIndex));
		}
		++OutSectionIndex;
		
	}

	GenerateProcMeshCapSection(Vertices, AllChunkTriangles, OutSectionIndex);

	// 물리 충돌체 생성용 버텍스 캐시
	CachedVertices = Vertices;
	
	ProcMeshComp->SetVisibility(true, true);
}

void AGuardianChunk::GenerateProcMeshCapSection(const TArray<FVector>& InVertices, const TArray<int32>& InTriangles,
                                                int32 CapSectionIndex)
{
	if (InVertices.Num() == 0 || InTriangles.Num() == 0)
	{
		PRINT_LOG(TEXT("No cap geometry provided"));
		return;
	}
	
    // 1. 에지 빈도수 계산 및 원본 폴리곤의 방향성 유지
    TMap<uint64, int32> EdgeCount;
    TArray<TPair<int32, int32>> DirectedEdges;

    for (int32 i = 0; i < InTriangles.Num(); i += 3)
    {
        for (int32 j = 0; j < 3; ++j)
        {
            int32 V0 = InTriangles[i + j];
            int32 V1 = InTriangles[i + (j + 1) % 3];

            // 방향에 무관한 고유 에지 키 생성
            uint64 EdgeKey = (static_cast<uint64>(FMath::Min(V0, V1)) << 32) | static_cast<uint64>(FMath::Max(V0, V1));

            if (EdgeCount.Contains(EdgeKey))
            {
                EdgeCount[EdgeKey]++;
            }
            else
            {
                EdgeCount.Add(EdgeKey, 1);
            }
            
            // 방향성을 가진 에지 기록
            DirectedEdges.Add(TPair<int32, int32>(V0, V1));
        }
    }

    // 2. 외곽선(빈도수가 1인 에지)만 필터링하여 '현재 정점 -> 다음 정점' 맵 구축
    TMap<int32, int32> NextVertexMap;
    for (const auto& Edge : DirectedEdges)
    {
        uint64 EdgeKey = (static_cast<uint64>(FMath::Min(Edge.Key, Edge.Value)) << 32) | static_cast<uint64>(FMath::Max(Edge.Key, Edge.Value));
        
        if (EdgeCount[EdgeKey] == 1)
        {
            NextVertexMap.Add(Edge.Key, Edge.Value);
        }
    }

    // 3. 그래프 탐색을 통해 독립된 다각형 루프(구멍) 추출
    TArray<TArray<int32>> HoleLoops;
    TSet<int32> VisitedVertices;

    for (const auto& Pair : NextVertexMap)
    {
        int32 StartVertex = Pair.Key;
        
        if (!VisitedVertices.Contains(StartVertex))
        {
            TArray<int32> CurrentLoop;
            int32 CurrentVertex = StartVertex;

            // 순환 고리가 닫힐 때까지 정점 추적
            while (!VisitedVertices.Contains(CurrentVertex) && NextVertexMap.Contains(CurrentVertex))
            {
                VisitedVertices.Add(CurrentVertex);
                CurrentLoop.Add(CurrentVertex);
                CurrentVertex = NextVertexMap[CurrentVertex];
            }
            
            if (CurrentLoop.Num() >= 3)
            {
                HoleLoops.Add(CurrentLoop);
            }
        }
    }

    // 4. 추출된 각 구멍(루프)별로 독립적인 단면 생성
    int32 CurrentSectionIndex = CapSectionIndex;

    for (const TArray<int32>& Loop : HoleLoops)
    {
        TArray<FVector> CapVertices;
        TArray<int32> CapTriangles;
        TArray<FVector> CapNormals;
        TArray<FVector2D> CapUVs;
        TArray<FColor> CapColors;
        TArray<FProcMeshTangent> CapTangents;

        // 해당 구멍만의 독립적인 중심점 산출
        FVector Centroid = FVector::ZeroVector;
        for (int32 VertexIndex : Loop)
        {
            Centroid += InVertices[VertexIndex];
        }
        Centroid /= static_cast<float>(Loop.Num());

        int32 StartIndex = CapVertices.Num();
        CapVertices.Add(Centroid);
        CapUVs.Add(FVector2D(0.5f, 0.5f));
        CapColors.Add(FColor::White);

        // 평면 법선 계산 (평가: 루프 정점들의 외적을 통해 정확히 계산해야 음영 오류가 없음)
        FVector CapNormal = FVector::UpVector; 
        FProcMeshTangent CapTangent = FProcMeshTangent(FVector::ForwardVector, false);

        for (int32 i = 0; i < Loop.Num(); ++i)
        {
            int32 VIndex = Loop[i];
            CapVertices.Add(InVertices[VIndex]);
            CapUVs.Add(FVector2D(0.0f, 0.0f));
            CapColors.Add(FColor::White);

            int32 NextI = (i + 1) % Loop.Num();
            
            // 와인딩 오더 역전 (외부를 향하도록 면 뒤집기)
            CapTriangles.Add(StartIndex);
            CapTriangles.Add(StartIndex + 1 + NextI); 
            CapTriangles.Add(StartIndex + 1 + i);
        }

        for (int32 i = 0; i < CapVertices.Num(); ++i)
        {
            CapNormals.Add(CapNormal);
            CapTangents.Add(CapTangent);
        }

        // 루프마다 새로운 섹션으로 분리 생성
        ProcMeshComp->CreateMeshSection(CurrentSectionIndex, CapVertices, CapTriangles, CapNormals, CapUVs, CapColors, CapTangents, true);
        
        if (CapMaterial)
        {
            ProcMeshComp->SetMaterial(CurrentSectionIndex, CapMaterial);
        }
        CurrentSectionIndex++;
    }
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
