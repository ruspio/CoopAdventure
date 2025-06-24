// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"

void PrintString(const FString& Str)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, Str);
    }    
}


UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
    PrintString("MSS Constructor");
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    PrintString("MSS Initialize");
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
    Super::Deinitialize();
    
    UE_LOG(LogTemp, Warning, TEXT("MSS Deinitialize"));
}
