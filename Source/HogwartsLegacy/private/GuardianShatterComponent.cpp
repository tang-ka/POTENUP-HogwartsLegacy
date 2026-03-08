// Fill out your copyright notice in the Description page of Project Settings.

#include "GuardianShatterComponent.h"

#include "HogwartsLegacy.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "../Plugins/Runtime/ProceduralMeshComponent/Source/ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "SkeletalRenderPublic.h"

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

UProceduralMeshComponent* UGuardianShatterComponent::GetOrCreateProceduralMesh()
{
	// 이미 생성했다면 그대로 반환
	if (ProceduralMeshComp)
	{
		return ProceduralMeshComp;
	}

	// 생성해야 하는데 Source Mesh가 없으면 nullptr 반환 (실패)
	if (!EnsureSourceMesh())
	{
		return nullptr;
	}

	AActor* Owner = GetOwner();

	ProceduralMeshComp = NewObject<UProceduralMeshComponent>(Owner, TEXT("GuardianProceduralMesh"));
	if (!ProceduralMeshComp)
	{
		PRINT_LOG(TEXT("Failed to create ProceduralMeshComponent"));
		return nullptr;
	}

	Owner->AddInstanceComponent(ProceduralMeshComp);
	ProceduralMeshComp->RegisterComponent();
	ProceduralMeshComp->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	ProceduralMeshComp->SetRelativeTransform(MeshComp->GetRelativeTransform());
	ProceduralMeshComp->SetCollisionEnabled(MeshComp->GetCollisionEnabled());
	ProceduralMeshComp->SetCollisionProfileName(MeshComp->GetCollisionProfileName());
	ProceduralMeshComp->SetCollisionObjectType(MeshComp->GetCollisionObjectType());
	ProceduralMeshComp->SetCanEverAffectNavigation(false);
	ProceduralMeshComp->SetCastShadow(MeshComp->CastShadow);
	ProceduralMeshComp->bUseComplexAsSimpleCollision = true;
	ProceduralMeshComp->SetVisibility(false, true);

	return ProceduralMeshComp;
}

void UGuardianShatterComponent::CopySkeletalMeshToProceduralMesh(UProceduralMeshComponent* InProceduralMeshComponent)
{
	if (!InProceduralMeshComponent || !EnsureSourceMesh())
	{
		return;
	}
	
	constexpr int32 LODIndex = 0;
	const FSkeletalMeshRenderData* RenderData = MeshComp->GetSkeletalMeshRenderData();	
	
	if (!RenderData || !RenderData->LODRenderData.IsValidIndex(LODIndex))
	{
		PRINT_LOG(TEXT("SkeletalMeshRenderData is invalid"));
		return;
	}
	
	const FSkeletalMeshLODRenderData& LODData = RenderData->LODRenderData[LODIndex];
	const FStaticMeshVertexBuffer& StaticMeshVertexBuffer = LODData.StaticVertexBuffers.StaticMeshVertexBuffer;

	TArray<FFinalSkinVertex> SkinnedVertices;
	MeshComp->GetCPUSkinnedVertices(SkinnedVertices, LODIndex);

	const int32 NumVertices = SkinnedVertices.Num();
	if (NumVertices <= 0)
	{
		PRINT_LOG(TEXT("Failed to read CPU skinned vertices. Check SkeletalMesh CPU access / skinning settings."));
		return;
	}

	if (NumVertices != LODData.GetNumVertices())
	{
		PRINT_LOG(TEXT("Skinned vertex count mismatch. Skinned: %d, RenderData: %d"), NumVertices, LODData.GetNumVertices());
		return;
	}

	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;
	Vertices.Reserve(NumVertices);
	Normals.Reserve(NumVertices);
	UV0.Reserve(NumVertices);
	VertexColors.Reserve(NumVertices);
	Tangents.Reserve(NumVertices);

	for (int32 VertexIndex = 0; VertexIndex < NumVertices; ++VertexIndex)
	{
		const FFinalSkinVertex& SkinnedVertex = SkinnedVertices[VertexIndex];
		
		const FVector3f& Position = SkinnedVertex.Position;
		const FVector3f Normal = SkinnedVertex.TangentZ.ToFVector3f();
		const FVector3f TangentX = SkinnedVertex.TangentX.ToFVector3f();
		const FVector2f UV = StaticMeshVertexBuffer.GetVertexUV(VertexIndex, 0);

		Vertices.Add(FVector(Position));
		Normals.Add(FVector(Normal));
		UV0.Add(FVector2D(UV));
		VertexColors.Add(FColor::White);
		Tangents.Add(FProcMeshTangent(FVector(TangentX), false));
	}

	FMultiSizeIndexContainerData IndicesData;
	LODData.MultiSizeIndexContainer.GetIndexBuffer(IndicesData.Indices);

	InProceduralMeshComponent->ClearAllMeshSections();

	for (int32 SectionIndex = 0; SectionIndex < LODData.RenderSections.Num(); ++SectionIndex)
	{
		const FSkelMeshRenderSection& RenderSection = LODData.RenderSections[SectionIndex];
		TArray<int32> Triangles;
		Triangles.Reserve(RenderSection.NumTriangles * 3);

		for (uint32 TriangleIndex = 0; TriangleIndex < RenderSection.NumTriangles * 3; ++TriangleIndex)
		{
			Triangles.Add(IndicesData.Indices[RenderSection.BaseIndex + TriangleIndex]);
		}

		InProceduralMeshComponent->CreateMeshSection(SectionIndex, Vertices, Triangles, Normals, UV0, VertexColors,
		                                            Tangents, true);

		if (MeshComp->GetNumMaterials() > RenderSection.MaterialIndex)
		{
			InProceduralMeshComponent->SetMaterial(SectionIndex, MeshComp->GetMaterial(RenderSection.MaterialIndex));
		}
	}

	InProceduralMeshComponent->SetVisibility(true, true);
}

void UGuardianShatterComponent::TriggerShatter()
{
	if (bIsShattered)
	{
		return;
	}

	// ProceduralMeshComponent이 이미 생성되어 있다면 그대로 사용, 아니라면 새로 생성
	UProceduralMeshComponent* LocalProceduralMeshComp = GetOrCreateProceduralMesh();
	if (!LocalProceduralMeshComp)
	{
		return;
	}

	// ProceduralMeshComponent에 SkeletalMesh의 데이터를 복사한다 (최소한 LOD0)
	CopySkeletalMeshToProceduralMesh(LocalProceduralMeshComp);

	//
	MeshComp->SetVisibility(false, true);
	MeshComp->SetHiddenInGame(true, true);
	LocalProceduralMeshComp->SetHiddenInGame(false, true);
	bIsShattered = true;
}
