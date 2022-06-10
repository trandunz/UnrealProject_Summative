// Copyright Epic Games, Inc. All Rights Reserved.

#include "GDP03_Summative2GameMode.h"
#include "GDP03_Summative2HUD.h"
#include "GDP03_Summative2Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/UnrealNetwork.h"

#define DISPLAY_LOG(fmt, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT(fmt), __VA_ARGS__));

void AGDP03_Summative2GameMode::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
}

void AGDP03_Summative2GameMode::BeginPlay()
{
	Super::BeginPlay();
}

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

void AGDP03_Summative2GameMode::MultiCast_DisableInput_Implementation()
{
	for (auto it = GetWorld()->GetPlayerControllerIterator(); it; it++)
	{
		APlayerController* controller = Cast<APlayerController>(it->Get());
		if (controller && controller->IsLocalController())
		{
			APawn* pawn = controller->GetPawn();
			if (pawn)
			{
				pawn->DisableInput(controller);
			}
		}
	}
}

bool AGDP03_Summative2GameMode::MultiCast_DisableInput_Validate()
{
	return true;
}
