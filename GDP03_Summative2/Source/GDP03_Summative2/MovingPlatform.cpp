// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Platform");
	Mesh->SetupAttachment(RootComponent);


	m_MoveAxis = { 1,0,0 };
	m_MoveDirection = 1;

	if (HasAuthority())
	{
		bReplicates = true;
		SetReplicatingMovement(true);
		Mesh->SetIsReplicated(true);
	}
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	m_StartLocation = GetActorLocation();
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		Movement();
	}
}

void AMovingPlatform::Movement()
{
	FVector location = GetActorLocation();

	if (m_MoveAxis.X != 0)
	{
		if (location.X < m_StartLocation.X - 200 || location.X > m_StartLocation.X + 200)
			m_MoveDirection *= -1;

		location.X += m_MoveDirection * MoveSpeed;
	}
	if (m_MoveAxis.Y != 0)
	{
		if (location.Y < m_StartLocation.Y - 200 || location.Y > m_StartLocation.Y + 200)
			m_MoveDirection *= -1;

		location.Y += m_MoveDirection * MoveSpeed;
	}
	if (m_MoveAxis.Z != 0)
	{
		if (location.Z < m_StartLocation.Z - 200 || location.Z > m_StartLocation.Z + 200)
			m_MoveDirection *= -1;

		location.Z += m_MoveDirection * MoveSpeed;
	}
	
	SetActorLocation(location);
}


