// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopAdventureGameState.h"
#include "Net/UnrealNetwork.h"
#include "CoopAdventureCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

ACoopAdventureGameState::ACoopAdventureGameState()
{
	bGameWon = false;
}

void ACoopAdventureGameState::OnRep_GameWon()
{
	if (!bGameWon) return;

	// Start timer to show UI after 2 seconds
	GetWorld()->GetTimerManager().SetTimer(
		VictoryUITimerHandle,
		this,
		&ACoopAdventureGameState::ShowVictoryUI,
		2.0f,
		false
	);	
}

void ACoopAdventureGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACoopAdventureGameState, bGameWon);
}

void ACoopAdventureGameState::ShowVictoryUI()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC || !PC->IsLocalController()) return;

	ACoopAdventureCharacter* Character = Cast<ACoopAdventureCharacter>(PC->GetPawn());
	if (!Character) return;

	PC->DisableInput(PC);

	if (Character->GetWinScreenWidgetClass() && !Character->GetWinScreenInstance())
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(PC, Character->GetWinScreenWidgetClass());
		if (Widget)
		{
			Character->SetWinScreenInstance(Widget);
			Widget->AddToViewport();

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(Widget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}
}