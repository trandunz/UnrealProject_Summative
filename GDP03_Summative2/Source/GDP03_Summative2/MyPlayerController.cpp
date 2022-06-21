// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

const FName SESSION_NAME = "UnrealSessionNameLeshgo";
TSharedPtr<class FOnlineSessionSearch> SearchSettings;

#define DISPLAY_LOG(fmt, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT(fmt), __VA_ARGS__));

AMyPlayerController::AMyPlayerController()
{
	// Get Active Subsystem and print it to console
	IOnlineSubsystem* subSystem = Online::GetSubsystem(GetWorld());
	UE_LOG(LogTemp, Warning, TEXT("[MyPlayerController] Found Subsystem %s"), *subSystem->GetSubsystemName().ToString());
}

void AMyPlayerController::Login()
{
	// Get Active Subsystem
	IOnlineSubsystem* subSystem = Online::GetSubsystem(GetWorld());
	if (subSystem)
	{
		IOnlineIdentityPtr identity = subSystem->GetIdentityInterface();
		if (identity.IsValid())
		{
			// Get local Player
			ULocalPlayer* localPlayer = Cast<ULocalPlayer>(Player);
			if (localPlayer != NULL)
			{
				// Cache Controller ID
				int controllerID = localPlayer->GetControllerId();

				// If they are not logged in
				if (identity->GetLoginStatus(controllerID) != ELoginStatus::LoggedIn)
				{
					// Set Login Complete Delegate
					identity->AddOnLoginCompleteDelegate_Handle(
						controllerID,
						FOnLoginCompleteDelegate::CreateUObject(this, &AMyPlayerController::OnLoginCompleteDelegate)
					);
					// Login with controller iD
					identity->AutoLogin(controllerID);
				}
			}
		}
	}
}

bool AMyPlayerController::HostSession(bool _lan)
{
	IOnlineSubsystem* subSystem;
	// Get lan if host is set to lan else get steam if available
	if (_lan) 
		subSystem = Online::GetSubsystem(GetWorld(), NULL_SUBSYSTEM);
	else
		subSystem = Online::GetSubsystem(GetWorld());

	if (subSystem)
	{
		IOnlineSessionPtr session = subSystem->GetSessionInterface();

		if (session.IsValid())
		{
			TSharedPtr<class FOnlineSessionSettings> sessionSettings = MakeShareable(new FOnlineSessionSettings());

			// Setup session settings
			sessionSettings->NumPrivateConnections = 6;
			sessionSettings->NumPublicConnections = 4;

			if (_lan)
				sessionSettings->bIsLANMatch = true;

			sessionSettings->bShouldAdvertise = true;
			sessionSettings->bAllowJoinInProgress = true;
			sessionSettings->bAllowInvites = true;
			sessionSettings->bUsesPresence = true;
			sessionSettings->bAllowJoinViaPresence = true;
			sessionSettings->bUseLobbiesIfAvailable = true;

			sessionSettings->Set(SEARCH_KEYWORDS, FString("Custom"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

			// Add On Create Session Delegate
			session->AddOnCreateSessionCompleteDelegate_Handle
			(
				FOnCreateSessionCompleteDelegate::CreateUObject(this, &AMyPlayerController::OnCreateSessionCompleteDelegate)
			);

			TSharedPtr<const FUniqueNetId> uniqueNetIdPtr = GetLocalPlayer()->GetPreferredUniqueNetId().GetUniqueNetId();

			// Create the session with given settings
			bool result = session->CreateSession(*uniqueNetIdPtr, SESSION_NAME, *sessionSettings);

			if (result)
			{
				DISPLAY_LOG("CreateSession: Success");
			}
			else
			{
				DISPLAY_LOG("CreateSession: Failed");
			}
		}
	}

	return false;
}

bool AMyPlayerController::FindSession(bool _lan)
{
	IOnlineSubsystem* subSystem;
	// Get lan if set to lan else get steam if available
	if (_lan)
	{
		IsLAN = true;
		subSystem = Online::GetSubsystem(GetWorld(), NULL_SUBSYSTEM);
	}
	else
	{
		IsLAN = false;
		subSystem = Online::GetSubsystem(GetWorld());
	}

	if (subSystem)
	{
		IOnlineSessionPtr session = subSystem->GetSessionInterface();
		if (session.IsValid())
		{
			// If Search Settings are already present, Reset it
			if (SearchSettings.IsValid())
			{
				SearchSettings.Reset();
			}
			SearchSettings = MakeShareable(new FOnlineSessionSearch());

			// Set query settings
			SearchSettings->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
			SearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
			SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, FString("Custom"), EOnlineComparisonOp::Equals);

			// Ad OnFindSessionDelegate
			session->AddOnFindSessionsCompleteDelegate_Handle
			(
				FOnFindSessionsCompleteDelegate::CreateUObject(this, &AMyPlayerController::OnFindSessionsCompleteDelegate)
			);

			TSharedRef<FOnlineSessionSearch> searchSettingsRef = SearchSettings.ToSharedRef();
			TSharedPtr<const FUniqueNetId> uniqueNetIdPtr = GetLocalPlayer()->GetPreferredUniqueNetId().GetUniqueNetId();

			// Find sessions
			return session->FindSessions(*uniqueNetIdPtr, searchSettingsRef);
		}
	}
	return false;
}

bool AMyPlayerController::JoinSession(bool _lan)
{
	if (SearchSettings->SearchResults.Num() > 0)
	{
		if (_lan)
			JoinSession(SearchSettings->SearchResults[0], true);
		else
			JoinSession(SearchSettings->SearchResults[0], false);
		
		return true;
	}
	return false;
}

void AMyPlayerController::JoinSession(FOnlineSessionSearchResult _searchResult, bool _lan)
{
	IOnlineSubsystem* subSystem;
	// Get lan if set to lan else get steam if available
	if (_lan)
		subSystem = Online::GetSubsystem(GetWorld(), NULL_SUBSYSTEM);
	else
		subSystem = Online::GetSubsystem(GetWorld());

	if (subSystem)
	{
		IOnlineSessionPtr session = subSystem->GetSessionInterface();
		if (session.IsValid())
		{
			if (_searchResult.IsValid())
			{
				// Add on join session delegate
				session->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &AMyPlayerController::OnJoinSessionCompleteDelegate));

				TSharedPtr<const FUniqueNetId> uniqueNetIdPtr = GetLocalPlayer()->GetPreferredUniqueNetId().GetUniqueNetId();

				// Join session
				session->JoinSession(*uniqueNetIdPtr, SESSION_NAME, _searchResult);

				DISPLAY_LOG("Joining Session!");
			}
			else
			{
				DISPLAY_LOG("Session Invalid!");
			}
		}
	}
}

void AMyPlayerController::QuitSession()
{
	IOnlineSubsystem* subSystem;
	// Get lan if set to lan else get steam if available
	if (IsLAN)
		subSystem = Online::GetSubsystem(GetWorld(), NULL_SUBSYSTEM);
	else
		subSystem = Online::GetSubsystem(GetWorld());

	if (subSystem)
	{
		IOnlineSessionPtr session = subSystem->GetSessionInterface();
		if (session.IsValid())
		{
			// Destory the session
			session->DestroySession(SESSION_NAME);
			// Open main Menu
			UGameplayStatics::OpenLevel
			(
				this,
				FName(TEXT("/Game/FirstPersonCPP/Maps/MainMenu")),
				true,
				""
			);
		}
	}
}

void AMyPlayerController::OnLoginCompleteDelegate(int32 _localUserNum, bool _wasSuccessful, const FUniqueNetId& _userID, const FString& _error)
{
	IOnlineIdentityPtr identity = Online::GetIdentityInterface();
	if (identity.IsValid())
	{
		ULocalPlayer* localPlayer = Cast<ULocalPlayer>(Player);
		if (localPlayer != NULL)
		{
			FUniqueNetIdRepl uniqueNetID = PlayerState->GetUniqueId();
			uniqueNetID.SetUniqueNetId(FUniqueNetIdWrapper(_userID).GetUniqueNetId());

			// Set Net ID
			PlayerState->SetUniqueId(uniqueNetID);

			// Display Login Status
			int controllerID = localPlayer->GetControllerId();
			ELoginStatus::Type status = identity->GetLoginStatus(controllerID);
			DISPLAY_LOG("Login Status: %s", ELoginStatus::ToString(status));
		}
	}
	else
	{
		UE_LOG_ONLINE(Warning, TEXT("Login Error: %s"), FCommandLine::Get());
		DISPLAY_LOG("Login Status: Identity Invalid");
	}
}

void AMyPlayerController::OnCreateSessionCompleteDelegate(FName _inSessionName, bool _wasSuccessful)
{
	if (_wasSuccessful)
	{
		// Open first Person Map
		UGameplayStatics::OpenLevel
		(
			this,
			FName(TEXT("/Game/FirstPersonCPP/Maps/FirstPersonExampleMap")),
			true,
			"listen"
		);
	}
}

void AMyPlayerController::OnFindSessionsCompleteDelegate(bool _wasSuccessful)
{
	if (_wasSuccessful)
	{
		if (SearchSettings->SearchResults.Num() == 0)
		{
			DISPLAY_LOG("No Sessions Found!");
		}
		else
		{
			DISPLAY_LOG("Session Found!");
		}
	}
	else
	{
		DISPLAY_LOG("Failed To Find Sessions!")
	}
}

void AMyPlayerController::OnJoinSessionCompleteDelegate(FName _sessionName, EOnJoinSessionCompleteResult::Type _result)
{
	IOnlineSubsystem* subSystem;
	if (IsLAN)
		subSystem = Online::GetSubsystem(GetWorld(), NULL_SUBSYSTEM);
	else
		subSystem = Online::GetSubsystem(GetWorld());

	if (subSystem)
	{
		IOnlineSessionPtr session = subSystem->GetSessionInterface();
		if (session.IsValid())
		{
			// If join session success
			if (_result == EOnJoinSessionCompleteResult::Success)
			{
				FString connectInfo;
				// Get connect info
				if (session->GetResolvedConnectString(SESSION_NAME, connectInfo))
				{
					// Travel To Session With Connect Info
					UE_LOG_ONLINE_SESSION(Log, TEXT("Joined Session!: Travelling to %s"), *connectInfo);
					AMyPlayerController::ClientTravel(connectInfo, TRAVEL_Absolute);
				}
			}
		}
	}
}
