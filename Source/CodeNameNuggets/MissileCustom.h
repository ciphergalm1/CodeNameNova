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

	void SetTarget(AActor* target);

	void Homing(AActor* target);

	void Fire();
	
};
