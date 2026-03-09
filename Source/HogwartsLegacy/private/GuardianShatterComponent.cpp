// Fill out your copyright notice in the Description page of Project Settings.

#include "GuardianShatterComponent.h"

#include "GuardianChunk.h"
#include "HogwartsLegacy.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "Rendering/SkeletalMeshRenderData.h"is as

UGuardianShatterComponent::UGuardianShatterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGuardianShatterComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeShatter();
}

void UGuardianShatterComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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
	for (int32 GroupIdx = 0; GroupIdx < ShatterBoneNames.Num(); ++GroupIdx)
	{
		const int32 BoneIdx = RefSkeleton.FindBoneIndex(ShatterBoneNames[GroupIdx]);
		if (BoneIdx != INDEX_NONE)
		{
			ShatterBoneIndexToGroupIndex.Add(BoneIdx, GroupIdx);
		}
		else
		{
			PRINT_LOG(TEXT("ShatterBone '%s' not found in skeleton"), *ShatterBoneNames[GroupIdx].ToString());
		}
	}

	OutBoneIndexToGroup.SetNumUninitialized(NumBones);

	for (int32 BoneIdx = 0; BoneIdx < NumBones; ++BoneIdx)
	{
		int32 CurrentIdx = BoneIdx;
		int32 FoundGroup = INDEX_NONE;

		// 자기 자신부터 루트까지 부모를 타고 올라간다
		while (CurrentIdx != INDEX_NONE)
		{
			if (const int32* GroupPtr = ShatterBoneIndexToGroupIndex.Find(CurrentIdx))
			{
				FoundGroup = *GroupPtr;
				break;
			}
			CurrentIdx = RefSkeleton.GetParentIndex(CurrentIdx);
		}

		OutBoneIndexToGroup[BoneIdx] = FoundGroup;
	}
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

	const int32 TotalVertices = LODData.GetNumVertices();
	const uint32 MaxInfluences = SkinWeightBuffer->GetMaxBoneInfluences();

	OutVertexToGroup.SetNumUninitialized(TotalVertices);

	for (int32 SectionIdx = 0; SectionIdx < LODData.RenderSections.Num(); ++SectionIdx)
	{
		const FSkelMeshRenderSection& Section = LODData.RenderSections[SectionIdx];
		const TArray<FBoneIndexType>& BoneMap = Section.BoneMap;

		for (uint32 LocalVertIdx = 0; LocalVertIdx < Section.NumVertices; ++LocalVertIdx)
		{
			const int32 GlobalVertIdx = Section.BaseVertexIndex + LocalVertIdx;

			uint16 BestWeight = 0;
			int32 BestGroup = INDEX_NONE;

			for (uint32 InflIdx = 0; InflIdx < MaxInfluences; ++InflIdx)
			{
				const uint16 Weight = SkinWeightBuffer->GetBoneWeight(GlobalVertIdx, InflIdx);
				if (Weight == 0)
				{
					break;
				}

				if (Weight > BestWeight)
				{
					// SkinWeightBuffer의 본 인덱스는 섹션-로컬 → BoneMap으로 전역 변환
					const uint32 SectionLocalBoneIdx = SkinWeightBuffer->GetBoneIndex(GlobalVertIdx, InflIdx);
					const int32 SkeletonBoneIdx = (SectionLocalBoneIdx < static_cast<uint32>(BoneMap.Num()))
						                              ? BoneMap[SectionLocalBoneIdx]
						                              : INDEX_NONE;

					if (SkeletonBoneIdx != INDEX_NONE && SkeletonBoneIdx < InBoneIndexToGroup.Num())
					{
						const int32 Group = InBoneIndexToGroup[SkeletonBoneIdx];
						if (Group != INDEX_NONE)
						{
							BestWeight = Weight;
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
	if (bIsShattered)
	{
		return;
	}

	if (!EnsureSourceMesh())
	{
		return;
	}

	if (ShatterBoneNames.Num() == 0)
	{
		PRINT_LOG(TEXT("ShatterBoneNames is empty"));
		return;
	}

	// 1) 본 → 그룹 맵 구축
	TArray<int32> BoneIndexToGroup;
	BuildBoneToGroupMap(BoneIndexToGroup);

	// 2) 버텍스 → 그룹 분류
	TArray<int32> VertexToGroup;
	ClassifyVerticesByBoneGroup(BoneIndexToGroup, VertexToGroup);

	// 3) 그룹별 버텍스 인덱스 수집
	const int32 NumGroups = ShatterBoneNames.Num();
	TArray<TSet<int32>> GroupVertexSets;
	GroupVertexSets.SetNum(NumGroups);

	for (int32 VertIdx = 0; VertIdx < VertexToGroup.Num(); ++VertIdx)
	{
		const int32 Group = VertexToGroup[VertIdx];
		if (Group != INDEX_NONE && Group < NumGroups)
		{
			GroupVertexSets[Group].Add(VertIdx);
		}
	}

	// 4) 그룹별 GuardianChunk 스폰
	const FTransform OwnerTransform = GetOwner()->GetActorTransform();
	for (int32 GroupIdx = 0; GroupIdx < NumGroups; ++GroupIdx)
	{
		if (GroupVertexSets[GroupIdx].Num() == 0)
		{
			PRINT_LOG(TEXT("Group '%s' has no vertices"), *ShatterBoneNames[GroupIdx].ToString());
			continue;
		}

		AGuardianChunk* Chunk = GetWorld()->SpawnActor<AGuardianChunk>(
			AGuardianChunk::StaticClass(), OwnerTransform);
		if (!Chunk)
		{
			PRINT_LOG(TEXT("Failed to spawn GuardianChunk for group '%s'"), *ShatterBoneNames[GroupIdx].ToString());
			continue;
		}

		Chunk->InitializeChunk(MeshComp, GroupVertexSets[GroupIdx]);
	}

	// 5) 원본 메시 숨기기
	MeshComp->SetVisibility(false, true);
	MeshComp->SetHiddenInGame(true, true);
	bIsShattered = true;
}
