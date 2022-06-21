// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Platform");
	Mesh->SetupAttachment(RootComponent);

	// Replicate if it has authority
	if (HasAuthority())
	{
		bReplicates = true;
		SetReplicatingMovement(true);
		Mesh->SetIsReplicated(true);
	}
}

void AMovingPlatform::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Relicate server state
	DOREPLIFETIME(AMovingPlatform, serverState);
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	// If it has authority, initialize variables
	if (HasAuthority())
	{
		m_StartLocation = GetActorLocation();
		m_MoveDirection = 1;
		m_ElapsedTime = 0.0f;
	}
}

FVector AMovingPlatform::SimulateMove(FMPMove move)
{
	// Return movement based on move axis
	if (MoveAxis.X != 0)
	{
		return FVector(move.movementAmplitude * cosf(move.time * move.movementSpeed), 0, 0);
	}
	if (MoveAxis.Y != 0)
	{
		return FVector(0, move.movementAmplitude * cosf(move.time * move.movementSpeed), 0);
	}
	else
		return FVector(0, 0, move.movementAmplitude * cosf(move.time * move.movementSpeed));
}

bool AMovingPlatform::IsLocallyControlled()
{
	const ENetMode NetMode = GetNetMode();

	if (NetMode == NM_Standalone)
	{
		// Not networked.
		return true;
	}

	if (NetMode == NM_Client && GetLocalRole() == ROLE_AutonomousProxy)
	{
		// Networked client in control.
		return true;
	}

	if (GetRemoteRole() != ROLE_AutonomousProxy && GetLocalRole() == ROLE_Authority)
	{
		// Local authority in control.
		return true;
	}

	return false;
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If Autonomous Proxy
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		// Add Elapsed Time
		m_ElapsedTime += DeltaTime;

		// Create A Move
		FMPMove move = CreateMove(DeltaTime);

		// Simulate / Execute The Move
		SetActorLocation(m_StartLocation + SimulateMove(move));

		// Add That Move To Moved Array
		Moves.Add(move);

		// Send The Move Too The Server
		Server_SendMove(move);
	}
	// If Authority
	if (HasAuthority() && IsLocallyControlled())
	{
		// Add Elapsed Time
		m_ElapsedTime += DeltaTime;

		// Create A Move
		FMPMove move = CreateMove(DeltaTime);

		// Simulate / Execute The Move
		SetActorLocation(m_StartLocation + SimulateMove(move));

		// Update Server State
		serverState.currentMove = move;
		serverState.transform = GetActorTransform();
	}
	// If Simulated Proxy
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		// Update Simualted Proxy
		ClientTick(DeltaTime);
	}
}

void AMovingPlatform::ClientTick(float DeltaTime)
{
	clientTimeSinceUpdate += DeltaTime;
	// Make sure lerp radio wont be messed up
	if (clientTimeSinceUpdate < KINDA_SMALL_NUMBER) { return; }
	float lerpRatio = clientTimeSinceUpdate / clientTimeBetweenLastUpdate;

	// Lerp Position
	FVector targetLocation = serverState.transform.GetLocation();
	FVector startLocation = clientStartTransform.GetLocation();
	FVector newlocaton = FMath::Lerp(startLocation, targetLocation, lerpRatio);
	SetActorLocation(newlocaton);

	// Lerp Rotation
	FQuat targetRotation = serverState.transform.GetRotation();
	FQuat startRotation = clientStartTransform.GetRotation();
	FQuat newRotation = FQuat::Slerp(startRotation, targetRotation, lerpRatio);
	SetActorRotation(newRotation);
}

FMPMove AMovingPlatform::CreateMove(float DeltaTime)
{
	FMPMove move;
	move.time = m_ElapsedTime;
	move.deltaTime = DeltaTime;
	move.movementSpeed = MoveSpeed;
	move.movementAmplitude = Amplitude;
	return move;
}

void AMovingPlatform::Server_SendMove_Implementation(FMPMove _move)
{
	// Simulate / Execute The Move
	SetActorLocation(m_StartLocation + SimulateMove(_move));

	// Update Server State
	serverState.currentMove = _move;
	serverState.transform = GetActorTransform();
}

bool AMovingPlatform::Server_SendMove_Validate(FMPMove _move)
{
	return true;
}

void AMovingPlatform::OnRep_ServerState()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		AutonomousProxy_OnRep_ServerState();

	}
	else if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		SimulatedProxy_OnRep_ServerState();
	}
}

void AMovingPlatform::SimulatedProxy_OnRep_ServerState()
{
	clientTimeBetweenLastUpdate = clientTimeSinceUpdate;
	clientTimeSinceUpdate = 0;

	clientStartTransform = GetActorTransform();
}

void AMovingPlatform::AutonomousProxy_OnRep_ServerState()
{
	//SetActorTransform(serverState.transform);

	// Cleanup move array of redundant moves
	TArray<FMPMove> newMoves;
	for (auto& move : Moves)
	{
		if (serverState.currentMove.time < move.time)
		{
			newMoves.Add(move);
		}
	}
	Moves = newMoves;

	// Simulate all remaining moves
	for (auto& move : Moves)
	{
		SimulateMove(move);
	}
}


