// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

USTRUCT()
struct FMPMove
{
	GENERATED_BODY()

	FMPMove() {}

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
struct FMPServerState
{
	GENERATED_BODY()

	FMPServerState() {}

	UPROPERTY()
		FMPMove currentMove;
	UPROPERTY()
		FTransform transform;
};

UCLASS()
class GDP03_SUMMATIVE2_API AMovingPlatform : public AActor
{
	GENERATED_BODY()
	
	FVector m_StartLocation;
	int m_MoveDirection;
	float m_ElapsedTime;

	float clientTimeSinceUpdate;
	float clientTimeBetweenLastUpdate;
	FTransform clientStartTransform;


public:	
	// Sets default values for this actor's properties
	AMovingPlatform();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<FMPMove> Moves;

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
		FMPServerState serverState;

	FVector SimulateMove(FMPMove move);

	bool IsLocallyControlled();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ClientTick(float DeltaTime);
	FMPMove CreateMove(float DeltaTime);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SendMove(FMPMove _move);

	UFUNCTION()
		void OnRep_ServerState();

	UFUNCTION()
		void SimulatedProxy_OnRep_ServerState();

	UFUNCTION()
		void AutonomousProxy_OnRep_ServerState();

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere)
		float MoveSpeed;

	UPROPERTY(EditAnywhere)
		float Amplitude;

	UPROPERTY(EditAnywhere)
		FVector MoveAxis;
};
