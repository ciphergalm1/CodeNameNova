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

	UPROPERTY(Category = mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* MissileTrailComponent;
	
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

	void EngageTarget(AActor* target);

private:

	// Manage target hit flag
	bool bHasHitTarget;

	bool bHasBeenFired;

	float fLifeTimeMax;

	float currentAirSpeed;

	float turnRate;

	void boosting();

	void SetTarget(AActor* target);

	void Homing(AActor* target);

	void Fire();
	
};
