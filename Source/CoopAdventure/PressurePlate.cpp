// Fill out your copyright notice in the Description page of Project Settings.


#include "PressurePlate.h"

// Sets default values
APressurePlate::APressurePlate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	SetReplicateMovement(true);

	Activated = false;

	TriggerPressurePlateXYScale = 3.3f;
	TriggerPressurePlateZScale = 0.2f;
	TriggerPressurePlateXYPosition = 0.f;
	TriggerPressurePlateZPosition = 10.f;

	MeshPressurePlateXYScale = 4.f;
	MeshPressurePlateZScale = 0.5f;
	MeshPressurePlateXYPosition = 0.f;
	MeshPressurePlateZPosition = 7.2f;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
	TriggerMesh->SetupAttachment(RootComp);
	TriggerMesh->SetIsReplicated(true);

	auto TriggerMeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(
		TEXT("/Game/StarterContent/Shapes/Shape_Cylinder")
	);
	if (TriggerMeshAsset.Succeeded())
	{
		TriggerMesh->SetStaticMesh(TriggerMeshAsset.Object);
		TriggerMesh->SetRelativeScale3D(FVector(
			TriggerPressurePlateXYScale, 
			TriggerPressurePlateXYScale, 
			TriggerPressurePlateZScale)
		);
		TriggerMesh->SetRelativeLocation(FVector(
			TriggerPressurePlateXYPosition,
			TriggerPressurePlateXYPosition,
			TriggerPressurePlateZPosition
		));
	}

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);

	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(
		TEXT("/Game/Stylized_Egypt/Meshes/building/SM_building_part_08")
	);
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object);
		Mesh->SetRelativeScale3D(FVector(
			MeshPressurePlateXYScale, 
			MeshPressurePlateXYScale, 
			MeshPressurePlateZScale)
		);
		Mesh->SetRelativeLocation(FVector(
			MeshPressurePlateXYPosition,
			MeshPressurePlateXYPosition,
			MeshPressurePlateZPosition
		));
	}
}

// Called when the game starts or when spawned
void APressurePlate::BeginPlay()
{
	Super::BeginPlay();

	TriggerMesh->SetVisibility(false);
	TriggerMesh->SetCollisionProfileName(FName("OverlapAll"));
}

// Called every frame
void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		TArray<AActor*> OverlappingActors;
		AActor* TriggerActor = 0;

		TriggerMesh->GetOverlappingActors(OverlappingActors);

		for (int ActorIdx = 0; ActorIdx < OverlappingActors.Num(); ++ActorIdx)
		{
			AActor* A = OverlappingActors[ActorIdx];
			if (A->ActorHasTag("TriggerActor"))
			{
				TriggerActor = A;
				break;
			}			

			//FString Msg = FString::Printf(TEXT("Name: %s"), *A->GetName());
			//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White, Msg);
		}
		if (TriggerActor)
		{
			if (!Activated)
			{
				Activated = true;
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White, TEXT("Activated"));
			}			
		} else {
			if (Activated)
			{
				Activated = false;
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White, TEXT("Deactivated"));
			}			
		}		
	}
}

void APressurePlate::SetActivated(bool bActivated)
{
	Activated = bActivated;
}

