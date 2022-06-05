// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <vector>
#include "ObjectiveItem.generated.h"

USTRUCT()
struct FObjectMove
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		float dt;
	UPROPERTY(EditAnywhere)
		FVector direction;
	UPROPERTY(EditAnywhere)
		FRotator rotation;
	UPROPERTY(EditAnywhere)
		float time;
	FObjectMove(float _dt, FVector _direction, FRotator _rotation,float _time )
	{
		time = _time;
		dt = _dt;
		direction = _direction;
		rotation = _rotation;
	}
	FObjectMove()
	{
		time = 0.0f;
		time = 0.0f;
		direction = { 0,0,0 };
		rotation = { 0,0,0 };
	}
};

USTRUCT()
struct FServerStateMove
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		FObjectMove lastMove;

	UPROPERTY(EditAnywhere)
		FTransform transform;

	UPROPERTY(EditAnywhere)
		FVector velocity;

	FServerStateMove()
	{

	}
};

UCLASS()
class GDP03_SUMMATIVE2_API AObjectiveItem : public AActor
{
	GENERATED_BODY()

	std::vector<FObjectMove> m_Moves;
	FVector m_StartLocation;
	int m_MoveDirection;

	void Orbit();
	void Bounce();
	FObjectMove CreateMove(float _dt);
	void SimulateMove(FObjectMove& _move);
	void Add(FObjectMove _move);
	
	bool IsLocallyControlled();

public:	

	// Sets default values for this actor's properties
	AObjectiveItem();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SendMove(FObjectMove _move);

	UFUNCTION()
		void OnRep_ServerState();

	UFUNCTION()
		void SimulatedProxy_OnRep_ServerState();

	UFUNCTION()
		void AutonomousProxy_OnRep_ServerState();

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UBoxComponent* TriggerCollider;
	UPROPERTY(EditAnywhere)
		float MoveSpeed;
	UPROPERTY(EditAnywhere)
		float Range;

	UPROPERTY(ReplicatedUsing = OnRep_ServerState, EditAnywhere)
		FServerStateMove ServerState;
};