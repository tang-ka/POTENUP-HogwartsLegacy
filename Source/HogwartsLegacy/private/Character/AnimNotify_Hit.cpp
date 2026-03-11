

#include "Character/AnimNotify_Hit.h"
#include "Character/PlayerCharacter.h"


void UAnimNotify_Hit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp)
	{
		return;
	}
	
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		if (MeshComp->GetOwner())
		{
			Player->bCanMove = true;
		}
	}
}
