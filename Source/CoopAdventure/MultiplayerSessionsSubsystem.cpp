// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"

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
    CreateServerAfterDestroy = false;
    DestroyServerName = "";
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool WasSuccessful)
{
    PrintString(FString::Printf(TEXT("OnCreateSessionComplete: %d"), WasSuccessful));

    if (WasSuccessful)
    {
        GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
    }    
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool WasSuccessful)
{
    PrintString(FString::Printf(TEXT("OnDestroySessionComplete: %d"), WasSuccessful));
    if (CreateServerAfterDestroy)
    {
        CreateServerAfterDestroy = false;
        CreateServer(DestroyServerName);
    }
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool WasSuccessful)
{
    if (!WasSuccessful) return;
    
    TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
    if (Results.Num() > 0)
    {
        FString Msg = FString::Printf(TEXT("%d sessions found"), Results.Num());
        PrintString(Msg);
    } else {
        PrintString("Zero sessions found.");
    }
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
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
                this, 
                &UMultiplayerSessionsSubsystem::OnCreateSessionComplete
            );

            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(
                this, 
                &UMultiplayerSessionsSubsystem::OnDestroySessionComplete
            );

            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(
                this, 
                &UMultiplayerSessionsSubsystem::OnFindSessionsComplete
            );
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

    FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(MySessionName);
    if (ExistingSession)
    {
        FString Msg = FString::Printf(
            TEXT("Session with name %s already exists, destroying it"), *MySessionName.ToString()
        );
        PrintString(Msg);
        CreateServerAfterDestroy = true;
        DestroyServerName = ServerName;
        SessionInterface->DestroySession(MySessionName);
        return;
    }

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

    if (ServerName.IsEmpty())
    {
        PrintString("Server name cannot be empty!");
        return;
    }
    
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    
    bool IsLAN = false;
    if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
    {
        IsLAN = true;
    } 

    SessionSearch->bIsLanQuery = IsLAN;
    SessionSearch->MaxSearchResults = 9999;
    SessionSearch->QuerySettings.Set(FName(TEXT("SEARCH_PRESENCE")), true, EOnlineComparisonOp::Equals);

    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}
