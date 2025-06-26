// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

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

void UMultiplayerSessionsSubsystem::CreateServer(FString ServerName)
{
    PrintString("CreateServer Function");
    
    if (ServerName.IsEmpty())
    {
        PrintString("Server name cannot be empty!");
        return;
    }

    FName MySessionName = FName("Co-op Adventure Session");

    FOnlineSessionSettings SessionSettings;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bIsDedicated = false;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.NumPublicConnections = 2;
    SessionSettings.bUseLobbiesIfAvailable = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bAllowJoinViaPresence = true;

    bool IsLAN = false;
    if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
    {
        IsLAN = true;
    }    
    SessionSettings.bIsLANMatch = IsLAN;

    SessionInterface->CreateSession(0, MySessionName, SessionSettings);
    
}

void UMultiplayerSessionsSubsystem::FindServer(FString ServerName)
{
    PrintString("FindServer Function");
}
