

#include "Character/AnimNotify_CheckAttackSpawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/PlayerCharacter.h"


void UAnimNotify_CheckAttackSpawn::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		UAnimMontage* PlayingMontage = Cast<UAnimMontage>(Animation);
		if (MeshComp->GetOwner())
		{
			if (PlayingMontage->GetName().Contains(TEXT("Atk")))
			{
				Player->SpawnBasicAttack();
				// Player->bCanMove = true;
			}
			if (PlayingMontage->GetName().Contains(TEXT("Protego")))
			{
				Player->SpawnProtego();
				// Player->bCanMove = true;
			}
			if (PlayingMontage->GetName().Contains(TEXT("Stupefy")))
			{
				Player->SpawnStupefy();
				// Player->bCanMove = true;
			}
		}
	}
}
