// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CoopAdventureGameState.generated.h"

UCLASS()
class COOPADVENTURE_API ACoopAdventureGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ACoopAdventureGameState();

	/** Czy gra została wygrana */
	UPROPERTY(ReplicatedUsing = OnRep_GameWon)
	bool bGameWon;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UFUNCTION()
	void OnRep_GameWon();
};