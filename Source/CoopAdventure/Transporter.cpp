// Fill out your copyright notice in the Description page of Project Settings.


#include "Transporter.h"
#include "PressurePlate.h"

// Sets default values for this component's properties
UTransporter::UTransporter()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	MoveTime = 3.f;
	ActivatedTriggerCount = 0;
	
	ArePointsSet = false;
	StartPoint = FVector::Zero();
	EndPoint = FVector::Zero();
}


// Called when the game starts
void UTransporter::BeginPlay()
{
	Super::BeginPlay();

	if (OwnerIsTriggerActor)
	{
		TriggerActors.Add(GetOwner());
	}	

	for (AActor* TA : TriggerActors)
	{
		APressurePlate* PressurePlateActor = Cast<APressurePlate>(TA);
		if (PressurePlateActor)
		{
			PressurePlateActor->OnActivated.AddDynamic(this, &UTransporter::OnPressurePlateActivated);
			PressurePlateActor->OnDeactivated.AddDynamic(this, &UTransporter::OnPressurePlateDeactivated);
		}		
	}	
}

void UTransporter::OnPressurePlateActivated()
{
	ActivatedTriggerCount++;

	FString Msg = FString::Printf(TEXT("Transporter Activated: %d"), ActivatedTriggerCount);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, Msg);
}

void UTransporter::OnPressurePlateDeactivated()
{
	ActivatedTriggerCount--;

	FString Msg = FString::Printf(TEXT("Transporter Activated: %d"), ActivatedTriggerCount);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, Msg);
}

// Called every frame
void UTransporter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TriggerActors.Num() > 0)
	{
		AllTriggerActorsTriggered = ActivatedTriggerCount >= TriggerActors.Num();
	}

	AActor* MyOwner = GetOwner();
	if (MyOwner && MyOwner->HasAuthority() && ArePointsSet)
	{
		FVector CurrentLocation = MyOwner->GetActorLocation();
		float Speed = FVector::Distance(StartPoint, EndPoint) / MoveTime;

		FVector TargetLocation = AllTriggerActorsTriggered ? EndPoint : StartPoint;
		if (!CurrentLocation.Equals(TargetLocation))
		{
			FVector NewLocation = FMath::VInterpConstantTo(
				CurrentLocation, TargetLocation, DeltaTime, Speed
			);
			MyOwner->SetActorLocation(NewLocation);
		}		
	}	
}

void UTransporter::SetPoints(FVector Point1, FVector Point2)
{
	if (Point1.Equals(Point2)) return;
	
	StartPoint = Point1;
	EndPoint = Point2;
	ArePointsSet = true;
}

void UTransporter::SetMoveTime(float fMoveTime)
{
	if (fMoveTime < 0.f)
	{
		MoveTime = 0.f;
		return;
	}
	
	MoveTime = fMoveTime;
}

void UTransporter::SetOwnerIsTriggerActor(bool bOwnerIsTriggerActor)
{
	OwnerIsTriggerActor = bOwnerIsTriggerActor;
}

