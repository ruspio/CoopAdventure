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
    ServerNameToFind = "";
    MySessionName = FName("Co-op Adventure Session");
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool WasSuccessful)
{
    PrintString(FString::Printf(TEXT("OnCreateSessionComplete: %d"), WasSuccessful));
    
    ServerCreateDel.Broadcast(WasSuccessful);

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
    if (ServerNameToFind.IsEmpty()) return;    
    
    TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
    FOnlineSessionSearchResult* CorrectResult = 0;

    if (Results.Num() > 0)
    {
        FString Msg = FString::Printf(TEXT("%d sessions found"), Results.Num());
        PrintString(Msg);

        for (FOnlineSessionSearchResult Result : Results)
        {
            if (Result.IsValid())
            {
                FString ServerName = "No-name";
                Result.Session.SessionSettings.Get(FName("SERVER_NAME"), ServerName);

                if (ServerName.Equals(ServerNameToFind))
                {
                    CorrectResult = &Result;

                    FString Msg2 = FString::Printf(TEXT("Found server with name: %s"), *ServerName);
                    PrintString(Msg2);
                    break;
                }                
            }
        }
        if (CorrectResult)
        {
            SessionInterface->JoinSession(0, MySessionName, *CorrectResult);
        } else {
            PrintString(FString::Printf(TEXT("Couldn't find server: %s"), *ServerNameToFind));
            ServerNameToFind = "";
            ServerJoinDel.Broadcast(false);
        }
    } else {
        PrintString("Zero sessions found.");
        ServerJoinDel.Broadcast(false);
    }
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    ServerJoinDel.Broadcast(Result == EOnJoinSessionCompleteResult::Success);
    
    if (Result == EOnJoinSessionCompleteResult::Success)
    {
        FString Msg = FString::Printf(TEXT("Successfully joined session %s"), *SessionName.ToString());
        PrintString(Msg);

        FString Address = "";
        bool Success = SessionInterface->GetResolvedConnectString(MySessionName, Address);
        if (Success)
        {
            PrintString(FString::Printf(TEXT("Addres: %s"), *Address));
            APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
            if (PlayerController)
            {
                PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
            }
            
        } else {
            PrintString("GetResolvedConnectString failed");
        }        
    } else {
        PrintString("OnJoinSessionComplete failed");
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

            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(
                this, 
                &UMultiplayerSessionsSubsystem::OnJoinSessionComplete
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
        ServerCreateDel.Broadcast(false);
        return;
    }

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

    SessionSettings.Set(
        FName("SERVER_NAME"), 
        ServerName, 
        EOnlineDataAdvertisementType::ViaOnlineServiceAndPing
    );

    SessionInterface->CreateSession(0, MySessionName, SessionSettings);
    
}

void UMultiplayerSessionsSubsystem::FindServer(FString ServerName)
{
    PrintString("FindServer Function");

    if (ServerName.IsEmpty())
    {
        PrintString("Server name cannot be empty!");
        ServerJoinDel.Broadcast(false);
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

    ServerNameToFind = ServerName;

    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}
