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
	 */
	void InitializeChunk(USkeletalMeshComponent* InSourceMeshComp, const TSet<int32>& InVertexIndices);
	
	UProceduralMeshComponent* GetProceduralMeshComponent() const { return ProcMeshComp; }

private:
	void CopySkeletalMeshToProceduralMesh(USkeletalMeshComponent* InSourceMeshComp, const TSet<int32>& InVertexIndices);
	void SetupChunkSettings(USkeletalMeshComponent* InSourceMeshComp);	

private:
	UPROPERTY()
	TObjectPtr<UProceduralMeshComponent> ProcMeshComp{};
};
