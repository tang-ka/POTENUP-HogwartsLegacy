

#include "Character/AnimNotify_Death.h"
#include "Components/SkeletalMeshComponent.h"
#include "UI/ControllerHUD.h"


void UAnimNotify_Death::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		if (MeshComp->GetOwner())
		{
			HUD = Cast<AControllerHUD>(MeshComp->GetWorld()->GetFirstPlayerController()->GetHUD());
			if (HUD)
			{
				HUD->Gameover();
			}
		}
	}
}
