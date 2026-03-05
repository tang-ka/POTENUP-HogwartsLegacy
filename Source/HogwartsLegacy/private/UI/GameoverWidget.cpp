

#include "UI/GameoverWidget.h"
#include "Kismet/GameplayStatics.h"


void UGameoverWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGameoverWidget::OnTryAgain()
{
	FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
}

void UGameoverWidget::OnQuit()
{
	auto* PC = GetWorld()->GetFirstPlayerController();
	UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
}
