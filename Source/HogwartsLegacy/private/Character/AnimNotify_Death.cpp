

#include "Character/AnimNotify_Death.h"
#include "Components/SkeletalMeshComponent.h"
#include "UI/ControllerHUD.h"


void UAnimNotify_Death::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                               const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	// if (APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	
	{
		// UAnimMontage* PlayingMontage = Cast<UAnimMontage>(Animation);
		if (MeshComp->GetOwner())
		{
			HUD = Cast<AControllerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
			if (HUD)
			{
				HUD->Gameover();
			}
		}
	}
}
