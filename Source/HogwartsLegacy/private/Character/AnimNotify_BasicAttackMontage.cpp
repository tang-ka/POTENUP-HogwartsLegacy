// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AnimNotify_BasicAttackMontage.h"
#include "Character/PlayerCharacter.h"

void UAnimNotify_BasicAttackMontage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	
	if (Player)
	{
		UAnimMontage* PlayingMontage = Cast<UAnimMontage>(Animation);
		if (MeshComp->GetOwner())
		{
			if (PlayingMontage->GetName().Contains(TEXT("Atk")))
			{
				Player->SpawnBasicAttack();
				Player->bCanMove = true;
			}
			if (PlayingMontage->GetName().Contains(TEXT("Protego")))
			{
				Player->SpawnProtego();
				Player->bCanMove = true;
			}
			if (PlayingMontage->GetName().Contains(TEXT("Stupefy")))
			{
				Player->SpawnStupefy();
				Player->bCanMove = true;
			}
		}
	}
}
