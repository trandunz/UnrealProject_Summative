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
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<class UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TriggerCollider = CreateDefaultSubobject<class UBoxComponent>(TEXT("Trigger"));
	TriggerCollider->SetBoxExtent(FVector(50.0f));
	TriggerCollider->SetGenerateOverlapEvents(true);

	TriggerCollider->SetupAttachment(RootComponent);
	TriggerCollider->SetHiddenInGame(true);

	if (HasAuthority())
	{
		bReplicates = true;
		//SetReplicatingMovement(true);
		Mesh->SetIsReplicated(true);
		TriggerCollider->SetIsReplicated(true);
	}
}

void AObjectiveItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AObjectiveItem, ServerState);
}

// Called when the game starts or when spawned
void AObjectiveItem::BeginPlay()
{
	Super::BeginPlay();

	m_StartLocation = GetActorLocation();
	m_MoveDirection = 1;
	MoveSpeed = 1.0f;
	Range = 5.0f;

	TriggerCollider->OnComponentBeginOverlap.RemoveDynamic(
		this, &AObjectiveItem::HandleOverlap);

	TriggerCollider->OnComponentBeginOverlap.AddDynamic(
		this, &AObjectiveItem::HandleOverlap);
}

// Called every frame
void AObjectiveItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		FObjectMove move = CreateMove(DeltaTime);
		SimulateMove(move);
		Add(move);
		Server_SendMove(move);
	}
	else if (HasAuthority() && IsLocallyControlled())
	{
		FObjectMove move = CreateMove(DeltaTime);
		Server_SendMove(move);
	}*/
	if (HasAuthority())
	{
		Bounce();
	}
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
			Destroy();
		}
	}
}

void AObjectiveItem::Orbit()
{
	SetActorRelativeLocation(GetActorLocation() + (MoveSpeed * GetActorRightVector() * Range));
	SetActorRelativeRotation(GetActorRotation() + FRotator(0, MoveSpeed, 0));
}

void AObjectiveItem::Bounce()
{
	FVector location = GetActorLocation();

	if (location.Z < m_StartLocation.Z - 100 || location.Z > m_StartLocation.Z + 100)
		m_MoveDirection *= -1;

	location.Z += m_MoveDirection * MoveSpeed;

	SetActorLocation(location);
}

FObjectMove AObjectiveItem::CreateMove(float _dt)
{
	return { _dt , FVector{0.0f,0.0f,(float)m_MoveDirection},GetActorRotation(), UGameplayStatics::GetRealTimeSeconds(GetWorld()) };
}

void AObjectiveItem::SimulateMove(FObjectMove& _move)
{
	FVector location = GetActorLocation();

	if (location.Z < m_StartLocation.Z - 100 || location.Z > m_StartLocation.Z + 100)
		m_MoveDirection *= -1;

	location += _move.direction * MoveSpeed;

	SetActorLocation(location);

	_move.direction = { 0,0,(float)m_MoveDirection };
	_move.rotation = GetActorRotation();
}

void AObjectiveItem::Add(FObjectMove _move)
{
	m_Moves.push_back(_move);
}

void AObjectiveItem::Server_SendMove_Implementation(FObjectMove _move)
{
	SimulateMove(_move);

	ServerState.lastMove = _move;
	ServerState.transform = GetActorTransform();
	ServerState.velocity = _move.direction * MoveSpeed;
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
	
}

void AObjectiveItem::AutonomousProxy_OnRep_ServerState()
{
	SetActorTransform(ServerState.transform);
	m_MoveDirection = ServerState.velocity.Normalize();

	for (std::vector<FObjectMove>::const_iterator move; move != m_Moves.end(); move++)
	{
		if (move->direction == ServerState.lastMove.direction
			&& move->rotation == ServerState.lastMove.rotation)
		{
			break;
		}
		else
		{
			m_Moves.erase(move);
		}
	}

	Add(ServerState.lastMove);

	for (auto& move : m_Moves)
	{
		SimulateMove(move);
	}
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

bool AObjectiveItem::Server_SendMove_Validate(FObjectMove _move)
{
	return true;
}
