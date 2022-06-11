// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectiveItem.generated.h"

USTRUCT()
struct FMove
{
	GENERATED_BODY()

	FMove() {}

	UPROPERTY()
		float movementSpeed;
	UPROPERTY()
		float movementAmplitude;
	UPROPERTY()
		float deltaTime;
	UPROPERTY()
		float time;
};

USTRUCT()
struct FServerState
{
	GENERATED_BODY()

		FServerState() {}

	UPROPERTY()
		FMove currentMove;
	UPROPERTY()
		FTransform transform;
};

UCLASS()
class GDP03_SUMMATIVE2_API AObjectiveItem : public AActor
{
	GENERATED_BODY()

public:	

	// Sets default values for this actor's properties
	AObjectiveItem();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector m_StartLocation;
	int m_MoveDirection;
	float m_ElapsedTime;

	float clientTimeSinceUpdate;
	float clientTimeBetweenLastUpdate;
	FTransform clientStartTransform;

	FMove Moves[5];

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FServerState serverState;

	FVector SimulateMove(FMove move);

	void AddMove(FMove _move);

	bool IsLocallyControlled();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ClientTick(float DeltaTime);

	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SendMove(FMove _move);

	UFUNCTION()
		void OnRep_ServerState();

	UFUNCTION()
		void SimulatedProxy_OnRep_ServerState();

	UFUNCTION()
		void AutonomousProxy_OnRep_ServerState();

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UBoxComponent* TriggerCollider;
	UPROPERTY(EditAnywhere)
		float MoveSpeed;
	UPROPERTY(EditAnywhere)
		float Amplitude;
};