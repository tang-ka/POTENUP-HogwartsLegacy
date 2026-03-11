// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GuardianChunk.h"
#include "GuardianShatterComponent.generated.h"

class USkeletalMesh;
class USkeletalMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOGWARTSLEGACY_API UGuardianShatterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGuardianShatterComponent();

protected:
	virtual void BeginPlay() override;

public:
	void InitializeShatter();
	
	UFUNCTION(BlueprintCallable)
	void TriggerShatter();
	
private:
	bool EnsureSourceMesh();

	/**
	 * 스켈레톤의 모든 본을 ShatterBoneNames 중 하나로 분류한다.
	 * 각 본에서 부모를 타고 올라가면서 가장 먼저 만나는 ShatterBoneName그룹.
	 * @param OutBoneIndexToGroup  스켈레톤 본 인덱스 → ShatterBoneNames 배열 인덱스 (분류 불가하면 INDEX_NONE)
	 */
	void BuildBoneToGroupMap(TArray<int32>& OutBoneIndexToGroup) const;

	/**
	 * 각 버텍스를 스킨 웨이트 기반으로 그룹에 분류한다.
	 * @param InBoneIndexToGroup  BuildBoneToGroupMap에서 생성한 본→그룹 맵
	 * @param OutVertexToGroup    전체 버텍스 인덱스 → 그룹 인덱스 (INDEX_NONE이면 미분류)
	 */
	void ClassifyVerticesByBoneGroup(const TArray<int32>& InBoneIndexToGroup, TArray<int32>& OutVertexToGroup) const;

	/**
	 * 현재 포즈 기준 공용 메시 스냅샷을 1회 수집한다.
	 * @param OutSnapshot  수집 결과를 저장할 공유 스냅샷
	 * @return 수집 성공 여부
	 */
	bool CollectSharedSnapshot(FSharedMeshSnapshot& OutSnapshot) const;

	/**
	 * 전체 삼각형을 1회 순회하여 그룹별 ChunkDescriptor를 생성한다.
	 * @param InVertexToGroup     전체 버텍스 → 그룹 인덱스 맵
	 * @param InGroupVertexSets   그룹별 버텍스 인덱스 집합
	 * @param OutDescriptors      그룹별 ChunkDescriptor 출력
	 */
	void BuildChunkDescriptors(const TArray<int32>& InVertexToGroup,
	                           const TArray<TSet<int32>>& InGroupVertexSets,
	                           TArray<FChunkDescriptor>& OutDescriptors) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Shatter)
	TObjectPtr<UMaterialInterface> CapMaterial;

	/** 폭발 힘의 세기 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Shatter)
	float ExplosionStrength = 0.5f;

	/** 폭발 반경 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Shatter)
	float ExplosionRadius = 10.f;
	
private:
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> MeshComp{};
	
	UPROPERTY()
	TObjectPtr<USkeletalMesh> Mesh{};
	
	/** 분리할 본 이름 배열. 배열 순서대로 계층 우선순위가 결정된다. (예: Spine, Neck → Spine 그룹, Neck 그룹) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Shatter, meta=(AllowPrivateAccess=true))
	TArray<FName> ShatterBoneGroup;
	
	bool bIsShattered{false};
	
	TArray<int32> BoneIndexToGroup;
	TArray<int32> VertexToGroup;
	TArray<TSet<int32>> GroupVertexSets;
};
