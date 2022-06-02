// Fill out your copyright notice in the Description page of Project Settings.


#include "ExitTrigger.h"
#include "DrawDebugHelpers.h"
#include "GDP03_Summative2Character.h"

#define DISPLAY_LOG(fmt, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT(fmt), __VA_ARGS__));

void AExitTrigger::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Purple, true, -1, 0, 5);
}

AExitTrigger::AExitTrigger()
{
	OnActorBeginOverlap.AddDynamic(this, &AExitTrigger::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &AExitTrigger::OnOverlapEnd);
}

void AExitTrigger::OnOverlapBegin(AActor* _overlapActor, AActor* _otherActor)
{
	if (_otherActor && _otherActor != this)
	{
		DISPLAY_LOG("Overlap Begin");
		AGDP03_Summative2Character* charactor = Cast< AGDP03_Summative2Character>(_otherActor);
		if (charactor)
		{
			if (charactor->CurrentObjective == "Get To Exit")
			{
				charactor->CurrentObjective = "You Win!";
			}
		}
	}
}

void AExitTrigger::OnOverlapEnd(AActor* _overlapActor, AActor* _otherActor)
{
	if (_otherActor && _otherActor != this)
	{
		DISPLAY_LOG("Overlap Ended");
	}
}
