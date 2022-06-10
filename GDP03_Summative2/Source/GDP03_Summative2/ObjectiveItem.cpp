// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GDP03_Summative2Character.h"
#include "Net/UnrealNetwork.h"

#define DISPLAY_LOG(fmt, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT(fmt), __VA_ARGS__));

// Sets default values
AObjectiveItem::AObjectiveItem()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<class UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TriggerCollider = CreateDefaultSubobject<class UBoxComponent>(TEXT("Trigger"));
	TriggerCollider->SetBoxExtent(FVector(50.0f));
	TriggerCollider->SetGenerateOverlapEvents(true);
	TriggerCollider->SetupAttachment(RootComponent);
	TriggerCollider->SetHiddenInGame(true);
	TriggerCollider->SetVisibility(false);

	if (HasAuthority())
	{
		bReplicates = true;
		SetReplicatingMovement(true);
		Mesh->SetIsReplicated(true);
		TriggerCollider->SetIsReplicated(true);
	}
}

void AObjectiveItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AObjectiveItem, serverState);
}

// Called when the game starts or when spawned
void AObjectiveItem::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		m_StartLocation = GetActorLocation();
		m_MoveDirection = 1;
		MoveSpeed = 1.0f;
		Amplitude = 50.0f;
		m_ElapsedTime = 0.0f;
	}

	TriggerCollider->OnComponentBeginOverlap.RemoveDynamic(
		this, &AObjectiveItem::HandleOverlap);

	TriggerCollider->OnComponentBeginOverlap.AddDynamic(
		this, &AObjectiveItem::HandleOverlap);
}

// Called every frame
void AObjectiveItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		m_ElapsedTime += DeltaTime;

		FMove move;
		move.time = m_ElapsedTime;
		move.deltaTime = DeltaTime;
		move.movementValue = MoveSpeed;
		move.movementAmplitude = Amplitude;

		FVector newLocation = SimulateMove(move);
		AddActorWorldOffset(newLocation * move.deltaTime);

		serverState.currentMove = move;
		serverState.transform = GetActorTransform();

		if (GetActorLocation().Z > m_StartLocation.Z + 50
			|| GetActorLocation().Z < m_StartLocation.Z - 50)
		{
			Amplitude *= -1;
		}
	}
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		ClientTick(DeltaTime);
	}
}

void AObjectiveItem::ClientTick(float DeltaTime)
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

void AObjectiveItem::HandleOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AGDP03_Summative2Character* playerCharacter = Cast< AGDP03_Summative2Character>(OtherActor);

	if (playerCharacter)
	{
		if (playerCharacter->CurrentObjective == "Retrieve Ball")
		{
			playerCharacter->CurrentObjective = "Get To Exit";
		}
	}
}

FVector AObjectiveItem::SimulateMove(FMove move)
{
	return FVector(0,0,move.movementAmplitude * move.movementValue);
}

void AObjectiveItem::Server_SendMove_Implementation(FMove _move)
{
}

void AObjectiveItem::OnRep_ServerState()
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

void AObjectiveItem::SimulatedProxy_OnRep_ServerState()
{
	clientTimeBetweenLastUpdate = clientTimeSinceUpdate;
	clientTimeSinceUpdate = 0;

	clientStartTransform = GetActorTransform();
}

void AObjectiveItem::AutonomousProxy_OnRep_ServerState()
{
	
}

bool AObjectiveItem::IsLocallyControlled()
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

bool AObjectiveItem::Server_SendMove_Validate(FMove _move)
{
	return true;
}
