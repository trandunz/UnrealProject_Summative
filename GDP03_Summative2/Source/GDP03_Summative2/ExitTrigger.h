// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "ExitTrigger.generated.h"

/**
 * 
 */
UCLASS()
class GDP03_SUMMATIVE2_API AExitTrigger : public ATriggerBox
{
	GENERATED_BODY()

protected:
	/// <summary>
	/// Gets called at start of play
	/// </summary>
	virtual void BeginPlay() override;

public:
	/// <summary>
	/// ExitTrigger Contructor
	/// </summary>
	AExitTrigger();

	UFUNCTION()
		/// <summary>
		/// Function to handle overlap begin
		/// </summary>
		/// <param name="_overlapActor"></param>
		/// <param name="_otherActor"></param>
		void OnOverlapBegin(class AActor* _overlapActor, class AActor* _otherActor);
};
