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
		bool HostSession();

private:
	void OnLoginCompleteDelegate(int32 _localUserNum, bool _wasSuccessful, const FUniqueNetId& _userID, const FString& _error);
	void OnCreateSessionCompleteDelegate(FName _inSessionName, bool _wasSuccessful);
};
