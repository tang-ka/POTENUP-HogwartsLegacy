

#include "Character/AnimNotify_CheckCombo.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/PlayerCharacter.h"


void UAnimNotify_CheckCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		UAnimMontage* PlayingMontage = Cast<UAnimMontage>(Animation);
		if (MeshComp->GetOwner())
		{
			Player->CheckCombo();
		}
	}
}
