// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Platform");
	RootComponent = Mesh;

	m_MoveDirection = 1;
	

	bReplicates = true;
	SetReplicateMovement(true);
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

	FVector location = GetActorLocation();

	if (location.X < m_StartLocation.X - 200 || location.X > m_StartLocation.X + 200)
		m_MoveDirection *= -1;

	location.X += m_MoveDirection * MoveSpeed;

	SetActorLocation(location);
}


