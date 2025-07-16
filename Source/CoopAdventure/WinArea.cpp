// Fill out your copyright notice in the Description page of Project Settings.


#include "WinArea.h"

#include "Components/BoxComponent.h"
#include "CoopAdventureCharacter.h"

// Sets default values
AWinArea::AWinArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SetReplicates(true);
	//SetReplicateMovement(true);

	WinAreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WinAreaBox"));
	SetRootComponent(WinAreaBox);

	WinCondition = false;
}

// Called when the game starts or when spawned
void AWinArea::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWinArea::MulticastRPCWin_Implementation()
{
	OnWinCondition.Broadcast();
}

// Called every frame
void AWinArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if (!WinCondition)
		{
			TArray<AActor*> OverlappingActors;		
			WinAreaBox->GetOverlappingActors(OverlappingActors, ACoopAdventureCharacter::StaticClass());

			WinCondition = (OverlappingActors.Num() == 2);
			if (WinCondition)
			{
				UE_LOG(LogTemp, Display, TEXT("Win!"));

				MulticastRPCWin();
			}
		}
	}
}
