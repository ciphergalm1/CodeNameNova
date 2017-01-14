// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CustomExplosion_Aircraft.generated.h"

UCLASS()
class CODENAMENUGGETS_API ACustomExplosion_Aircraft : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACustomExplosion_Aircraft();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// end play event
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason ) override;

private:
	// Particle system component for managing the explosion effect
	UPROPERTY(Category = Effect, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* ExplosionEffectComponent;

	// Audiocomponent for managing the sound effect
	UPROPERTY(Category = Effect, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* ExplosionSoundComponent;

	// for handling the self destruction
	//FTimerHandle explosionHandle;
	
};
