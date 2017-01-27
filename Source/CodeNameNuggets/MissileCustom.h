// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MissileCustom.generated.h"

UCLASS()
class CODENAMENUGGETS_API AMissileCustom : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Category = mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MissileMeshComponent;

	UPROPERTY(Category = VFX, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* MissileTrailComponent;

	UPROPERTY(Category = SFX, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* MissileSoundComponent;
	
public:

	// override the notifyhit function
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	// Sets default values for this actor's properties
	AMissileCustom();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(Category = target, BlueprintReadWrite)
	class AActor* currentTarget;

	void EngageTarget(AActor* target, APawn* newMissileOwner);

	APawn* MissileOwner;

	void SetDamage(float val);

private:

	// Manage target hit flag
	bool bHasHitTarget;

	float SelfDestructionTimer;

	bool bHasBeenFired;

	bool bStartDestruction;

	bool bHasTarget;

	float fLifeTime;

	float fLifeTimeMax;

	float currentAirSpeed;

	float turnRate;

	void Boosting();

	void SetTarget(AActor* target);

	void Homing(AActor* target);

	void Fire();

	void SelfDestruction();

	void SpawnExplosion();

	float damage;


	
};
