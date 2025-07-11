// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Transporter.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPADVENTURE_API UTransporter : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTransporter();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	FVector StartPoint;
	FVector EndPoint;
	
	UPROPERTY(VisibleAnywhere)
	bool ArePointsSet;

	UPROPERTY(EditAnywhere)
	float MoveTime;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> TriggerActors;
	
	UPROPERTY(VisibleAnywhere)
	int ActivatedTriggerCount;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool AllTriggerActorsTriggered = false;
	
	UFUNCTION()
	void OnPressurePlateActivated();
	
	UFUNCTION()
	void OnPressurePlateDeactivated();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool GetAllTriggerActorsTriggered() { return AllTriggerActorsTriggered; }

	UFUNCTION()
	void SetPoints(FVector Point1, FVector Point2);
};
