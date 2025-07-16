// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WinArea.generated.h"

class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWinAreaOnWinCondition);

UCLASS()
class COOPADVENTURE_API AWinArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWinArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* WinAreaBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool WinCondition;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void MulticastRPCWin();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FWinAreaOnWinCondition OnWinCondition;

};
