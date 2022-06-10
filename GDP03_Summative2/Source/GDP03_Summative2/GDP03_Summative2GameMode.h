// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GDP03_Summative2GameMode.generated.h"

UCLASS(minimalapi)
class AGDP03_Summative2GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AGDP03_Summative2GameMode();

	UFUNCTION()
		void OnMissionComplete(APawn* _intigatorPawn);

protected:

	
};



