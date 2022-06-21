// Copyright Epic Games, Inc. All Rights Reserved.

#include "GDP03_Summative2Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "GDP03_Summative2Character.h"


AGDP03_Summative2Projectile::AGDP03_Summative2Projectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AGDP03_Summative2Projectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	// Replicate it it is on server
	if (HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);
	}
}

void AGDP03_Summative2Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{

		AGDP03_Summative2Character* mCharacter = Cast< AGDP03_Summative2Character>(OtherActor);
		if (mCharacter != nullptr)
		{
			if (GetLocalRole() == ROLE_Authority)
			{
				// Decrease PPlayer Health By 10
				if (mCharacter->CurrentHealth > 0)
					mCharacter->CurrentHealth -= 10;

				// Check if player died
				// If true, disable player input
				if (mCharacter->CurrentHealth <= 0)
				{
					APlayerController* controller = Cast<APlayerController>(mCharacter->GetController());
					if (controller)
					{
						APawn* mPawn = controller->GetPawn();
						if (mPawn)
						{
							mPawn->DisableInput(controller);
						}
					}
					
				}
			}
		}
	}
	Destroy();
}