// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"

void PrintString(const FString& Str)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, Str);
    }    
}

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
    //PrintString("MSS Constructor");
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    //PrintString("MSS Initialize");
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        FString SubsystemName = OnlineSubsystem->GetSubsystemName().ToString();
        PrintString(SubsystemName);

        SessionInterface =  OnlineSubsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            PrintString("Session Interface is valid!");
        }
    }
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
    Super::Deinitialize();
    
    //UE_LOG(LogTemp, Warning, TEXT("MSS Deinitialize"));
}
