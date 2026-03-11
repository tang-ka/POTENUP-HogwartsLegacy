

#include "Character/AnimNotify_CheckAttackSpawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/PlayerCharacter.h"


void UAnimNotify_CheckAttackSpawn::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
		if (!AnimInst)
		{
			return;
		}
		UAnimMontage* PlayingMontage = AnimInst->GetCurrentActiveMontage();
		if (!PlayingMontage)
		{
			return;
		}

		const FString MontageName = PlayingMontage->GetName();

		if (MontageName.Contains(TEXT("Atk")))
		{
			Player->SpawnBasicAttack();
			Player->bCanMove = true;
			Player->bCanAttack = true;
		}
		else if (MontageName.Contains(TEXT("Protego")))
		{
			Player->SpawnProtego();
			Player->bCanMove = true;
			Player->bCanAttack = true;
		}
		else if (MontageName.Contains(TEXT("Stupefy")))
		{
			Player->SpawnStupefy();
			Player->bCanMove = true;
			Player->bCanAttack = true;
		}
	}
}
