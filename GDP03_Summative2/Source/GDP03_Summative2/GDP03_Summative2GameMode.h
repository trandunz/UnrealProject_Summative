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

	virtual void Tick(float _DeltaTime) override;

	virtual void BeginPlay() override;

	AGDP03_Summative2GameMode();

protected:

	UFUNCTION(NetMultiCast, Reliable, WithValidation)
		void MultiCast_DisableInput();
};



