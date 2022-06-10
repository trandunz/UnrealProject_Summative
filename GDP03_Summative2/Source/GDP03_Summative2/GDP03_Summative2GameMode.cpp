// Copyright Epic Games, Inc. All Rights Reserved.

#include "GDP03_Summative2GameMode.h"
#include "GDP03_Summative2HUD.h"
#include "GDP03_Summative2Character.h"
#include "MyGameState.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/UnrealNetwork.h"

#define DISPLAY_LOG(fmt, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT(fmt), __VA_ARGS__));

AGDP03_Summative2GameMode::AGDP03_Summative2GameMode()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AGDP03_Summative2HUD::StaticClass();
}

void AGDP03_Summative2GameMode::OnMissionComplete(APawn* _intigatorPawn)
{
	AGDP03_Summative2Character* character = Cast< AGDP03_Summative2Character>(_intigatorPawn->GetController()->GetCharacter());
	if (character)
	{
		character->HasWon = true;
	}

	AMyGameState* gameState = Cast<AMyGameState>(GetWorld()->GetGameState());
	if (gameState)
	{
		gameState->GameOver = true;
		gameState->MultiCast_DisableInput();
	}
}
