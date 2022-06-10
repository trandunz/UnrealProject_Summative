// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GDP03_Summative2HUD.generated.h"

UCLASS()
class AGDP03_Summative2HUD : public AHUD
{
	GENERATED_BODY()

public:
	AGDP03_Summative2HUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:

};

