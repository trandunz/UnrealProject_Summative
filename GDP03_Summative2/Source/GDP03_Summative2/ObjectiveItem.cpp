// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GDP03_Summative2Character.h"

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
		SetReplicatingMovement(true);
		Mesh->SetIsReplicated(true);
		TriggerCollider->SetIsReplicated(true);
	}
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

