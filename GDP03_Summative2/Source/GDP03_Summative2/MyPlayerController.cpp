// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

const FName SESSION_NAME = "SessionName";
TSharedPtr<class FOnlineSessionSearch> searchSettings;

#define DISPLAY_LOG(fmt, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT(fmt), __VA_ARGS__));

AMyPlayerController::AMyPlayerController()
{
	IOnlineSubsystem* subSystem = Online::GetSubsystem(GetWorld());

	UE_LOG(LogTemp, Warning, TEXT("[MyPlayerController] Found Subsystem %s"), *subSystem->GetSubsystemName().ToString());
}

void AMyPlayerController::Login()
{
	IOnlineSubsystem* subSystem = Online::GetSubsystem(GetWorld());
	if (subSystem)
	{
		IOnlineIdentityPtr identity = subSystem->GetIdentityInterface();
		if (identity.IsValid())
		{
			ULocalPlayer* localPlayer = Cast<ULocalPlayer>(Player);
			if (localPlayer != NULL)
			{
				int controllerID = localPlayer->GetControllerId();
				if (identity->GetLoginStatus(controllerID) != ELoginStatus::LoggedIn)
				{
					identity->AddOnLoginCompleteDelegate_Handle(
						controllerID,
						FOnLoginCompleteDelegate::CreateUObject(this, &AMyPlayerController::OnLoginCompleteDelegate)
					);

					identity->AutoLogin(controllerID);
				}
			}
		}
	}
}

bool AMyPlayerController::HostSession()
{
	IOnlineSubsystem* subSystem = Online::GetSubsystem(GetWorld());
	bool result{false};

	if (subSystem)
	{
		IOnlineSessionPtr session = subSystem->GetSessionInterface();

		if (session.IsValid())
		{
			TSharedPtr<class FOnlineSessionSettings> sessionSettings = MakeShareable(new FOnlineSessionSettings());

			sessionSettings->NumPrivateConnections = 6;
			sessionSettings->NumPublicConnections = 4;

			sessionSettings->bShouldAdvertise = true;
			sessionSettings->bAllowJoinInProgress = true;
			sessionSettings->bAllowInvites = true;
			sessionSettings->bUsesPresence = true;
			sessionSettings->bAllowJoinViaPresence = true;
			sessionSettings->bUseLobbiesIfAvailable = true;

			sessionSettings->Set(SEARCH_KEYWORDS, FString("Custom"), EOnlineDataAdvertisementType::ViaOnlineService);

			session->AddOnCreateSessionCompleteDelegate_Handle
			(
				FOnCreateSessionCompleteDelegate::CreateUObject(this, &AMyPlayerController::OnCreateSessionCompleteDelegate)
			);

			TSharedPtr<const FUniqueNetId> uniqueNetIdPtr = GetLocalPlayer()->GetPreferredUniqueNetId().GetUniqueNetId();

			result = session->CreateSession(*uniqueNetIdPtr, SESSION_NAME, *sessionSettings);

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

	return result;
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

			PlayerState->SetUniqueId(uniqueNetID);

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
		UGameplayStatics::OpenLevel
		(
			this,
			FName(TEXT("/Game/FirstPersonCPP/Maps/FirstPersonExampleMap")),
			true,
			"listen"
		);
	}
}
