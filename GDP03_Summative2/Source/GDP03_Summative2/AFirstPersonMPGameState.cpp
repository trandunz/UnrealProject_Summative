// Fill out your copyright notice in the Description page of Project Settings.


#include "AFirstPersonMPGameState.h"
#include "GDP03_Summative2Character.h"

void AFirstPersonMPGameState::MultiCastOnMissionComplete(APawn* InstigatorPawn)
{
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; it++)
	{
		AGDP03_Summative2Character* pController = Cast< AGDP03_Summative2Character>(it->Get());
		if (pController && pController->IsLocallyControlled())
		{

		}
	}
}