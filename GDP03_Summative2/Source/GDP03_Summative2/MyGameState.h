// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

UCLASS()
class GDP03_SUMMATIVE2_API AMyGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(NetMultiCast, Reliable)
		void MultiCast_DisableInput();


	UPROPERTY(Replicated, BluePrintReadOnly)
		bool GameOver = false;
};
