// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GDP03_SUMMATIVE2_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

		AMyPlayerController();

public:
	UFUNCTION(BlueprintCallable, Category = "OnlineSession")
		void Login();

	UFUNCTION(BlueprintCallable, Category = "OnlineSession")
		bool HostSession(bool _lan);

	UFUNCTION(BlueprintCallable, Category = "OnlineSession")
		bool FindSession(bool _lan);

	UFUNCTION(BlueprintCallable, Category = "OnlineSession")
		bool JoinSession(bool _lan);

	UFUNCTION(BlueprintCallable, Category = "OnlineSession")
		void QuitSession();

private:
	void JoinSession(class FOnlineSessionSearchResult _searchResult, bool _lan);

	void OnLoginCompleteDelegate(int32 _localUserNum, bool _wasSuccessful, const FUniqueNetId& _userID, const FString& _error);
	void OnCreateSessionCompleteDelegate(FName _inSessionName, bool _wasSuccessful);
	void OnFindSessionsCompleteDelegate(bool _wasSuccessful);
	void OnJoinSessionCompleteDelegate(FName _sessionName, EOnJoinSessionCompleteResult::Type _result);

	bool IsLAN{ false };
};
