// Fill out your copyright notice in the Description page of Project Settings.

#include "GuardianShatterComponent.h"

#include "GuardianChunk.h"
#include "HogwartsLegacy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/EnemyFSMComponent.h"
#include "Enemy/Guardian.h"
#include "GameFramework/Character.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "ProfilingDebugging/CPUProfilerTrace.h"

UGuardianShatterComponent::UGuardianShatterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	ShatterBoneGroup =
	{
		FName("Spine"), 
		FName("Neck"),
		FName("RightArm"),
		FName("LeftArm"),
		FName("RightUpLeg"),
		FName("LeftUpLeg"),
	};
}

void UGuardianShatterComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeShatter();
	
	// 1) 본 → 그룹 맵 구축
	BuildBoneToGroupMap(BoneIndexToGroup);

	// 2) 버텍스 → 그룹 분류
	ClassifyVerticesByBoneGroup(BoneIndexToGroup, VertexToGroup);
	
	// 3) 그룹별 버텍스 인덱스 수집
	const int32 NumGroups = ShatterBoneGroup.Num();
	GroupVertexSets.SetNum(NumGroups);

	for (int32 Index = 0; Index < VertexToGroup.Num(); ++Index)
	{
		const int32 Group = VertexToGroup[Index];
		if (Group != INDEX_NONE && Group < NumGroups)
		{
			GroupVertexSets[Group].Add(Index);
		}
	}
	
	CastChecked<AGuardian>(GetOwner())->GetFSMComponent()->OnEnemyDead.AddUObject(this, &UGuardianShatterComponent::TriggerShatter);
}

void UGuardianShatterComponent::InitializeShatter()
{
	MeshComp = nullptr;
	Mesh = nullptr;

	const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		PRINT_LOG(TEXT("Owner is not a Character"));
		return;
	}

	MeshComp = OwnerCharacter->GetMesh();
	if (!MeshComp)
	{
		PRINT_LOG(TEXT("MeshComp is NULL"));
		return;
	}

	Mesh = MeshComp->GetSkeletalMeshAsset();
	if (!Mesh)
	{
		PRINT_LOG(TEXT("SkeletalMeshAsset is NULL"));
	}
}

bool UGuardianShatterComponent::EnsureSourceMesh()
{
	if (MeshComp && Mesh)
	{
		return true;
	}

	InitializeShatter();
	return MeshComp && Mesh;
}

// ---------------------------------------------------------------------------
// 본 계층 분류:
//   각 스켈레톤 본에 대해, 자기 자신부터 루트까지 부모를 타고 올라가면서
//   ShatterBoneNames 중 하나를 처음 만나면 그 그룹에 소속시킨다.
//   예) ShatterBoneNames = { Spine, Neck }
//       Neck의 자식 Head → Head에서 올라감 → Neck 발견 → Neck 그룹(인덱스 1)
//       Spine의 자식 Spine1 → Spine1에서 올라감 → Spine 발견 → Spine 그룹(인덱스 0)
// ---------------------------------------------------------------------------
void UGuardianShatterComponent::BuildBoneToGroupMap(TArray<int32>& OutBoneIndexToGroup) const
{
	const FReferenceSkeleton& RefSkeleton = Mesh->GetRefSkeleton();
	const int32 NumBones = RefSkeleton.GetNum();

	// ShatterBoneNames → 스켈레톤 본 인덱스 룩업 (빠른 검색용)
	TMap<int32, int32> ShatterBoneIndexToGroupIndex;
	for (int32 GroupIdx = 0; GroupIdx < ShatterBoneGroup.Num(); ++GroupIdx)
	{
		const int32 BoneIdx = RefSkeleton.FindBoneIndex(ShatterBoneGroup[GroupIdx]);
		if (BoneIdx != INDEX_NONE)
		{
			ShatterBoneIndexToGroupIndex.Add(BoneIdx, GroupIdx);
		}
		else
		{
			PRINT_LOG(TEXT("ShatterBone '%s' not found in skeleton"), *ShatterBoneGroup[GroupIdx].ToString());
		}
	}

	OutBoneIndexToGroup.SetNumUninitialized(NumBones);

	// DP방식 O(N) -> RefSkeleton에서 부모 본은 항상 자식 본보다 인덱스가 작으므로 
	// 앞에서부터 순회하면서 부모 그룹 정보를 이용해 자식 그룹을 채운다
	for (int32 BoneIdx = 0; BoneIdx < NumBones; ++BoneIdx)
	{
		int32 FoundGroup = INDEX_NONE;

		if (const int32* GroupIndexPtr = ShatterBoneIndexToGroupIndex.Find(BoneIdx))
		{
			FoundGroup = *GroupIndexPtr;
		}
		else
		{
			const int32 ParentIdx = RefSkeleton.GetParentIndex(BoneIdx);
			if (ParentIdx != INDEX_NONE)
			{
				FoundGroup = OutBoneIndexToGroup[ParentIdx];
			}
		}

		OutBoneIndexToGroup[BoneIdx] = FoundGroup;
	}

	// O(N * D) 방식 -> 최악: N^2 (N=본 개수, D=본 계층 깊이)
	// for (int32 BoneIdx = 0; BoneIdx < NumBones; ++BoneIdx)
	// {
	// 	int32 CurrentIdx = BoneIdx;
	// 	int32 FoundGroup = INDEX_NONE;
	// 	
	// 	// 자기 자신부터 루트까지 부모를 타고 올라간다
	// 	while (CurrentIdx != INDEX_NONE)
	// 	{
	// 		if (const int32* GroupIndexPtr = ShatterBoneIndexToGroupIndex.Find(CurrentIdx))
	// 		{
	// 			FoundGroup = *GroupIndexPtr;
	// 			break;
	// 		}
	// 		CurrentIdx = RefSkeleton.GetParentIndex(CurrentIdx);
	// 	}
	//
	// 	OutBoneIndexToGroup[BoneIdx] = FoundGroup;
	// }
}

// ---------------------------------------------------------------------------
// 버텍스 분류:
//   각 버텍스의 스킨 웨이트를 읽어, 가장 큰 웨이트를 가진 본의 그룹으로 분류.
//   SkinWeightVertexBuffer의 본 인덱스는 섹션 로컬이므로
//   FSkelMeshRenderSection::BoneMap으로 전역 본 인덱스로 변환 후 분류.
// ---------------------------------------------------------------------------
void UGuardianShatterComponent::ClassifyVerticesByBoneGroup(
	const TArray<int32>& InBoneIndexToGroup, TArray<int32>& OutVertexToGroup) const
{
	constexpr int32 LODIndex = 0;
	const FSkeletalMeshRenderData* RenderData = MeshComp->GetSkeletalMeshRenderData();
	if (!RenderData || !RenderData->LODRenderData.IsValidIndex(LODIndex))
	{
		return;
	}

	const FSkeletalMeshLODRenderData& LODData = RenderData->LODRenderData[LODIndex];
	const FSkinWeightVertexBuffer* SkinWeightBuffer = LODData.GetSkinWeightVertexBuffer();
	if (!SkinWeightBuffer)
	{
		PRINT_LOG(TEXT("SkinWeightBuffer is NULL"));
		return;
	}

	const TArray<FSkelMeshRenderSection>& RenderSections = LODData.RenderSections;

	const int32 VerticesNum = LODData.GetNumVertices();
	const uint32 MaxInfluences = SkinWeightBuffer->GetMaxBoneInfluences();

	OutVertexToGroup.SetNumUninitialized(VerticesNum);

	for (int32 SectionIdx = 0; SectionIdx < RenderSections.Num(); ++SectionIdx)
	{
		const FSkelMeshRenderSection& CurSection = RenderSections[SectionIdx];
		const TArray<FBoneIndexType>& CurBoneMap = CurSection.BoneMap;

		for (uint32 LocalVertIdx = 0; LocalVertIdx < CurSection.NumVertices; ++LocalVertIdx)
		{
			const int32 GlobalVertIdx = CurSection.BaseVertexIndex + LocalVertIdx;

			uint16 BestWeight = 0;
			int32 BestGroup = INDEX_NONE;

			for (uint32 InflIdx = 0; InflIdx < MaxInfluences; ++InflIdx)
			{
				const uint16 CurWeight = SkinWeightBuffer->GetBoneWeight(GlobalVertIdx, InflIdx);
				if (CurWeight == 0)
				{
					break;
				}

				if (CurWeight > BestWeight)
				{
					// SkinWeightBuffer의 본 인덱스는 섹션-로컬 → BoneMap으로 전역 변환
					const uint32 SectionLocalBoneIdx = SkinWeightBuffer->GetBoneIndex(GlobalVertIdx, InflIdx);
					const int32 SkeletonBoneIdx = (SectionLocalBoneIdx < static_cast<uint32>(CurBoneMap.Num()))
						                              ? CurBoneMap[SectionLocalBoneIdx]
						                              : INDEX_NONE;

					if (SkeletonBoneIdx != INDEX_NONE && SkeletonBoneIdx < InBoneIndexToGroup.Num())
					{
						const int32 Group = InBoneIndexToGroup[SkeletonBoneIdx];
						if (Group != INDEX_NONE)
						{
							BestWeight = CurWeight;
							BestGroup = Group;
						}
					}
				}
			}

			OutVertexToGroup[GlobalVertIdx] = BestGroup;
		}
	}
}

void UGuardianShatterComponent::TriggerShatter()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(GuardianShatterComponent::TriggerShatter);
	if (bIsShattered)
	{
		return;
	}

	if (!EnsureSourceMesh())
	{
		return;
	}

	if (ShatterBoneGroup.Num() == 0)
	{
		PRINT_LOG(TEXT("ShatterBoneNames is empty"));
		return;
	}

	// 1) 본 → 그룹 맵 구축
	// TArray<int32> BoneIndexToGroup;
	// BuildBoneToGroupMap(BoneIndexToGroup);
	
	// 2) 버텍스 → 그룹 분류
	// TArray<int32> VertexToGroup;
	// ClassifyVerticesByBoneGroup(BoneIndexToGroup, VertexToGroup);

	// 3) 그룹별 버텍스 인덱스 수집
	// const int32 NumGroups = ShatterBoneGroup.Num();
	// TArray<TSet<int32>> GroupVertexSets;
	// GroupVertexSets.SetNum(NumGroups);
	//
	// for (int32 Index = 0; Index < VertexToGroup.Num(); ++Index)
	// {
	// 	const int32 Group = VertexToGroup[Index];
	// 	if (Group != INDEX_NONE && Group < NumGroups)
	// 	{
	// 		GroupVertexSets[Group].Add(Index);
	// 	}
	// }

	// 4) 그룹별 GuardianChunk 스폰
	const int32 NumGroups = ShatterBoneGroup.Num();
	const FTransform OwnerTransform = GetOwner()->GetActorTransform();
	const FVector ExplosionCenter = GetOwner()->GetActorLocation();

	TArray<AGuardianChunk*> SpawnedChunks;

	for (int32 GroupIdx = 0; GroupIdx < NumGroups; ++GroupIdx)
	{
		if (GroupVertexSets[GroupIdx].Num() == 0)
		{
			PRINT_LOG(TEXT("Group '%s' has no vertices"), *ShatterBoneGroup[GroupIdx].ToString());
			continue;
		}

		AGuardianChunk* Chunk = GetWorld()->SpawnActor<AGuardianChunk>(AGuardianChunk::StaticClass(), OwnerTransform);
		if (!Chunk)
		{
			PRINT_LOG(TEXT("Failed to spawn GuardianChunk for group '%s'"), *ShatterBoneGroup[GroupIdx].ToString());
			continue;
		}

		Chunk->InitializeChunk(MeshComp, GroupVertexSets[GroupIdx], CapMaterial);
		SpawnedChunks.Add(Chunk);
	}

	// 5) 원본 메시 숨기기
	MeshComp->SetVisibility(false, true);
	MeshComp->SetHiddenInGame(true, true);
	bIsShattered = true;

	// 6) 각 Chunk에 순차적 지연 프레임을 부여하여 프레임당 1개씩 물리 활성화
	//    (Convex Hull 쿠킹 비용을 분산시켜 스파이크 방지)
	//    Guardian이 Destroy되어도 Chunk는 독립 Actor이므로 자체 Tick에서 안전하게 처리됨
	for (int32 i = 0; i < SpawnedChunks.Num(); ++i)
	{
		SpawnedChunks[i]->SchedulePhysicsActivation(ExplosionCenter, ExplosionStrength, ExplosionRadius, i);
	}
}
