// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectiveItem.generated.h"



UCLASS()
class GDP03_SUMMATIVE2_API AObjectiveItem : public AActor
{
	GENERATED_BODY()
	
public:	

	DECLARE_EVENT(AObjectiveItem, FObjectiveItemEvent)
	FObjectiveItemEvent TriggerOverlapBeginEvent;

	// Sets default values for this actor's properties
	AObjectiveItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
		virtual void TriggerCallbackOn(AActor* OtherActor);

	void Orbit();

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UBoxComponent* TriggerCollider;
	UPROPERTY(EditAnywhere)
		float MoveSpeed;
	UPROPERTY(EditAnywhere)
		float Range;
};
