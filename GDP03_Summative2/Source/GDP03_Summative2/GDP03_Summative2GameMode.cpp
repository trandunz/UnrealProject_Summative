// Copyright Epic Games, Inc. All Rights Reserved.

#include "GDP03_Summative2GameMode.h"
#include "GDP03_Summative2HUD.h"
#include "GDP03_Summative2Character.h"
#include "UObject/ConstructorHelpers.h"

AGDP03_Summative2GameMode::AGDP03_Summative2GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AGDP03_Summative2HUD::StaticClass();
}
