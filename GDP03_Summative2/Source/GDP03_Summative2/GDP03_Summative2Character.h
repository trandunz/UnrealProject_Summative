// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GDP03_Summative2Character.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

USTRUCT()
struct FPMove
{
	GENERATED_BODY()

		FPMove() {}

	UPROPERTY()
		FVector moveDirection;
	UPROPERTY()
		FRotator rotation;
	UPROPERTY()
		float deltaTime;
	UPROPERTY()
		float time;
};

USTRUCT()
struct FPServerState
{
	GENERATED_BODY()

		FPServerState() {}

	UPROPERTY()
		FPMove currentMove;
	UPROPERTY()
		FTransform transform;
	UPROPERTY()
		FVector velocity;
};

UCLASS(config=Game)
class AGDP03_Summative2Character : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* L_MotionController;

	bool EnableInput = true;

	float m_ElapsedTime;

public:
	AGDP03_Summative2Character();

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AGDP03_Summative2Projectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 bUsingMotionControllers : 1;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, EditAnywhere, BlueprintReadWrite)
		float CurrentHealth;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentObjective, EditAnywhere, BlueprintReadWrite)
		FString CurrentObjective;

	UPROPERTY(ReplicatedUsing = OnRep_HasWon, EditAnywhere, BlueprintReadWrite)
		bool HasWon;

	UPROPERTY(ReplicatedUsing = OnRep_IsGameOver, EditAnywhere, BlueprintReadWrite)
		bool IsGameOver;

	UFUNCTION()
		void EndTheGame(bool _hasWon);

	float clientTimeSinceUpdate;
	float clientTimeBetweenLastUpdate;
	FTransform clientStartTransform;
	FVector clientStartVelocity;

	FPMove Moves[5];

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FPServerState serverState;

	FVector SimulateMove(FPMove move);

	void AddMove(FPMove _move);

	void ClientTick(float DeltaTime);

protected:
	
	virtual void Tick(float _deltaTime);

	/** Fires a projectile. */
	void OnFire();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_OnFire();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_PlayerDeath();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SendMove(FPMove _move);

	UFUNCTION(NetMultiCast, Reliable, WithValidation)
		void MultiCast_PlayerDeath();

	UFUNCTION()
		void OnRep_CurrentHealth();

	UFUNCTION()
		void OnRep_CurrentObjective();

	UFUNCTION()
		void OnRep_HasWon();

	UFUNCTION()
		void OnRep_IsGameOver();

	UFUNCTION()
		void OnRep_ServerState();

	UFUNCTION()
		void AutonomousProxy_OnRep_ServerState();

	UFUNCTION()
		void SimulatedProxy_OnRep_ServerState();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

