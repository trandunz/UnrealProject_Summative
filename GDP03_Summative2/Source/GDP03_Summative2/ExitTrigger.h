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
	virtual void BeginPlay() override;

public:
	AExitTrigger();

	UFUNCTION()
		void OnOverlapBegin(class AActor* _overlapActor, class AActor* _otherActor);
};
