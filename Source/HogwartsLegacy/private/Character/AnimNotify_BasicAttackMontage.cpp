// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AnimNotify_BasicAttackMontage.h"
#include "Character/PlayerCharacter.h"

void UAnimNotify_BasicAttackMontage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (MeshComp && MeshComp->GetOwner())
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
		if (Player)
		{
			Player->SpawnBasicAttack();
			Player->bCanMove = true;
		}
	}
}
