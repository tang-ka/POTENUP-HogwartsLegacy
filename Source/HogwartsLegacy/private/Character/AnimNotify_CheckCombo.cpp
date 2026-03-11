

#include "Character/AnimNotify_CheckCombo.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/PlayerCharacter.h"


void UAnimNotify_CheckCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		Player->CheckCombo();
	}
}
