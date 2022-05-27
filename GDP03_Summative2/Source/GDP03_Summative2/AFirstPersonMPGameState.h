// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UCLASS()
class GDP03_SUMMATIVE2_API AFirstPersonMPGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	UFUNCTION(NetMulticast, Reliable)
		void MultiCastOnMissionComplete(APawn* InstigatorPawn);
};
