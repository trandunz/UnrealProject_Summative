// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameState.h"
#include "Net/UnrealNetwork.h"

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameState, GameOver);
}

void AMyGameState::MultiCast_DisableInput_Implementation()
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