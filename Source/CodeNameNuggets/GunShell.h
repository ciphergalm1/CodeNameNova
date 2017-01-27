// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GunShell.generated.h"

UCLASS()
class CODENAMENUGGETS_API AGunShell : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Category = Shell, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Shell;

	UPROPERTY(Category = Shell, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* ShellTrail;
	
public:	
	// Sets default values for this actor's properties
	AGunShell();

	// override the notifyhit function
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void SetDamage(float val);

private:

	float ShellSpeed;

	float damage;

	// handle shell movement
	void shellTravel();

	void SpawnExplosion();

	
};
