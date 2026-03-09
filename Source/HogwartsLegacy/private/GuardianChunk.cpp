// Fill out your copyright notice in the Description page of Project Settings.

#include "GuardianChunk.h"

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
}

void AGuardianChunk::InitializeChunk(USkeletalMeshComponent* InSourceMeshComp, const TSet<int32>& InVertexIndices)
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

	CopySkeletalMeshToProceduralMesh(InSourceMeshComp, InVertexIndices);
	SetupChunkSettings(InSourceMeshComp);
}

void AGuardianChunk::CopySkeletalMeshToProceduralMesh(USkeletalMeshComponent* InSourceMeshComp, const TSet<int32>& InVertexIndices)
{
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

	int32 OutSectionIndex = 0;
	for (int32 SectionIndex = 0; SectionIndex < LODData.RenderSections.Num(); ++SectionIndex)
	{
		const FSkelMeshRenderSection& RenderSection = LODData.RenderSections[SectionIndex];

		// 이 섹션의 삼각형 중, 3개 꼭짓점 모두 우리 그룹에 속하는 것만 수집
		TArray<int32> Triangles;
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

		ProcMeshComp->CreateMeshSection(OutSectionIndex, Vertices, Triangles, Normals, UV0, VertexColors,
		                                Tangents, true);

		if (InSourceMeshComp->GetNumMaterials() > static_cast<int32>(RenderSection.MaterialIndex))
		{
			ProcMeshComp->SetMaterial(OutSectionIndex, InSourceMeshComp->GetMaterial(RenderSection.MaterialIndex));
		}
		++OutSectionIndex;
	}

	ProcMeshComp->SetVisibility(true, true);
}

void AGuardianChunk::SetupChunkSettings(USkeletalMeshComponent* InSourceMeshComp)
{
	ProcMeshComp->SetRelativeTransform(InSourceMeshComp->GetComponentTransform());
	ProcMeshComp->SetCollisionEnabled(InSourceMeshComp->GetCollisionEnabled());
	ProcMeshComp->SetCollisionProfileName(InSourceMeshComp->GetCollisionProfileName());
	ProcMeshComp->SetCollisionObjectType(InSourceMeshComp->GetCollisionObjectType());
	ProcMeshComp->SetCanEverAffectNavigation(false);
	ProcMeshComp->SetCastShadow(InSourceMeshComp->CastShadow);
	ProcMeshComp->bUseComplexAsSimpleCollision = true;
	ProcMeshComp->SetVisibility(true, true);
}