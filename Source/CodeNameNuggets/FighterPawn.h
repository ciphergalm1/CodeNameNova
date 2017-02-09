// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "FighterPawn.generated.h"

UCLASS()
class CODENAMENUGGETS_API AFighterPawn : public APawn
{
	GENERATED_BODY()

	/** StaticMesh component that will be the visuals for our flying pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PlaneMesh;

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* EngineSoundComponent;

	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* CannonSoundComponent;

	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* LockingSoundComponent;

	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* LockedSoundComponent;

	UPROPERTY(Category = VFX, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* AfterBurnerComponent;

	/*
	UPROPERTY(Category = PlayerContoller, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class APlayerController* CurrentPlayerController;
	*/

public:
	// Sets default values for this pawn's properties
	AFighterPawn();

	// Begin AActor overrides
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	// Called when the game starts or when spawned
	// End AActor overrides

protected:

	// Begin APawn overrides
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override; // Allows binding actions/axes to functions
	// End APawn overrides

	/** Bound to the thrust axis */
	void ThrustInput(float Val);

	/** Bound to the vertical axis */
	void MoveUpInput(float Val);

	/** Bound to the horizontal axis */
	void MoveRightInput(float Val);

	/** Bound to the Yaw axis */
	void MoveYawInput(float Val);

	/** Bound to camera Y axis */
	void CameraUpInput(float Val);

	/** Bound to camera X axis */
	void CameraRightInput(float Val);

	/** Search for target if current target is null */
	void SearchTarget();

	/** Bound to the Fire Missile button */
	void FireMissile();

	/** Bound to the Fire Guns button*/
	void FireGuns();

	void ShootGuns();

	void StopFireGuns();

	void ToggleCannonSound();

	bool isFiringCannon;

	bool canFireCannon;

	bool bMissileOnLeftPylon;

	UForceFeedbackEffect* EngineViberation;

	/*
	UPROPERTY(Category =  Spawn, BlueprintReadWrite)
	TSubclassOf<class Custom> Explosion;
	*/

private:

	// Player is alive or not
	UPROPERTY(Category = Plane, EditAnywhere)
	bool isAlive;

	/** How quickly forward speed changes */
	UPROPERTY(Category = Plane, EditAnywhere)
	float Acceleration;

	/** How quickly pawn can steer */
	UPROPERTY(Category = Plane, EditAnywhere)
	float TurnSpeed;

	/** Max forward speed */
	UPROPERTY(Category = Pitch, EditAnywhere)
	float MaxSpeed;

	/** Min forward speed */
	UPROPERTY(Category = Yaw, EditAnywhere)
	float MinSpeed;

	/** Speed Delta */
	float SpeedDelta;

	/** Current forward speed */
	float CurrentForwardSpeed;

	/** Current yaw speed */
	float CurrentYawSpeed;

	/** Current pitch speed */
	float CurrentPitchSpeed;

	/** Current roll speed */
	float CurrentRollSpeed;

	/** Current Camera Pitch */
	float CurrentCameraPitch;

	/** Current Camera Yaw */
	float CurrentCameraYaw;

	/** Set up thurst */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MinThrust;

	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxThrust;

	UPROPERTY(Category = Plane, BlueprintReadOnly, meta  = (AllowPrivateAccess = "true"))
	float CurrentThrust;

	UPROPERTY(Category = Plane, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrentThrustRatio;

	/** Set Plane Health */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float aircraftHP;

	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Score;

	/** Set Aircraft ALT and vector */
	UPROPERTY(Category = Plane, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrentHeading;

	UPROPERTY(Category = Plane, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrentAltitude;

	UPROPERTY(Category = Plane, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrentPitchAngle;

	UPROPERTY(Category = Plane, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float NormalAirSpeed;

	UPROPERTY(Category = Weapon, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DetectionDistance;

	UPROPERTY(Category = Weapon, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DetectionRadius;

	UPROPERTY(Category = Weapon, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MissileRemain;

	UPROPERTY(Category = Weapon, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MissileDamage;

	struct FCollisionShape DetectionShape;

	UPROPERTY(Category = Plane, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class APawn* CurrentTarget;

	UPROPERTY(Category = Plane, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AEnemyPawn* targetSelected;

	void ClearSelectTarget();

	FTimerHandle GunCoolHandle;

	float LockOnGauge;

	void LockOnTarget();

	void ResetGunCool();

public:

	/** Returns PlaneMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetPlaneMesh() const { return PlaneMesh; }
	/** Returns SpringArm subobject **/
	FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }

	//return the status of the aircraft for UI
	UFUNCTION(Category = UI, BlueprintPure)
	float GetAirSpeed() const;

	//return the status of the aircraft for UI
	UFUNCTION(Category = UI, BlueprintPure)
	int GetAltitude() const;

	//return the status of the aircraft for UI
	UFUNCTION(Category = UI, BlueprintPure)
	float GetThrust() const;

	//return the status of the aircraft for UI
	UFUNCTION(Category = UI, BlueprintPure)
	int GetBearing() const;

	UFUNCTION(Category = UI, BlueprintPure)
	int GetHealth() const;

	UFUNCTION(Category = UI, BlueprintPure)
	int GetMissile() const;

	UFUNCTION(Category = UI, BlueprintPure)
	int GetScore() const;

	UFUNCTION(Category = UI, BlueprintPure)
	float GetPitch() const;

	UFUNCTION(Category = UI, BlueprintPure)
	float GetRoll() const;

	UFUNCTION(Category = UI, BlueprintCallable)
	void SetScore(float val);

	UFUNCTION(Category = Sound, BlueprintCallable)
	void SetSoundVolume(float volume);

	UFUNCTION(Category = Sound, BlueprintCallable)
	void SetSoundRefNull();

	UFUNCTION(Category = Damage, BlueprintCallable)
	void ReceiveDamage(float damageVal);

	UFUNCTION(Category = UI, BlueprintPure)
	class APawn* GetCurrentTarget();


private:
	// Handling the explosion
	void SpawnExplosion();

	// Handling Sound ManageMent
	void ConfigEngineSound();

	// Handling Engine Viberation
	void ConfigEngineViberation();

	// Handling AfterBurner Effect
	void ConfigAfterBurner();

	

	
};
