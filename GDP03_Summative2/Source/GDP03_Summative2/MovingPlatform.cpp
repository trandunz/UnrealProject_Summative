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

	DOREPLIFETIME(AMovingPlatform, serverState);
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		m_StartLocation = GetActorLocation();
		m_MoveDirection = 1;
		m_ElapsedTime = 0.0f;
	}
}

FVector AMovingPlatform::SimulateMove(FMPMove move)
{
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

void AMovingPlatform::AddMove(FMPMove _move)
{
	int size = sizeof(Moves) / sizeof(FMPMove);
	Moves[(size + 1) % 5] = _move;
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

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		m_ElapsedTime += DeltaTime;

		FMPMove move;
		move.time = m_ElapsedTime;
		move.deltaTime = DeltaTime;
		move.movementSpeed = MoveSpeed;
		move.movementAmplitude = Amplitude;

		AddMove(move);

		Server_SendMove(move);
	}
	if (HasAuthority() && IsLocallyControlled())
	{
		m_ElapsedTime += DeltaTime;

		FMPMove move;
		move.time = m_ElapsedTime;
		move.deltaTime = DeltaTime;
		move.movementSpeed = MoveSpeed;
		move.movementAmplitude = Amplitude;

		Server_SendMove(move);
	}
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		ClientTick(DeltaTime);
	}
}

void AMovingPlatform::ClientTick(float DeltaTime)
{
	clientTimeSinceUpdate += DeltaTime;
	if (clientTimeSinceUpdate < KINDA_SMALL_NUMBER) { return; }
	float lerpRatio = clientTimeSinceUpdate / clientTimeBetweenLastUpdate;

	FVector targetLocation = serverState.transform.GetLocation();
	FVector startLocation = clientStartTransform.GetLocation();
	FVector newlocaton = FMath::Lerp(startLocation, targetLocation, lerpRatio);
	SetActorLocation(newlocaton);

	FQuat targetRotation = serverState.transform.GetRotation();
	FQuat startRotation = clientStartTransform.GetRotation();
	FQuat newRotation = FQuat::Slerp(startRotation, targetRotation, lerpRatio);
	SetActorRotation(newRotation);
}

void AMovingPlatform::Server_SendMove_Implementation(FMPMove _move)
{
	FVector newLocation = SimulateMove(_move);
	AddActorWorldOffset(newLocation * _move.deltaTime);

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
	SetActorTransform(serverState.transform);

	int sizeOfMoves = sizeof(Moves) / sizeof(FMPMove);
	int location = 0;
	for (int i = 0; i < sizeOfMoves; i++)
	{
		if (SimulateMove(Moves[i]) == SimulateMove(serverState.currentMove))
		{
			break;
		}
		else
		{
			location++;
			Moves[i] = FMPMove();
		}
	}

	for (int i = location; i < sizeOfMoves; i++)
	{
		SetActorLocation(SimulateMove(Moves[i]));
	}
}


