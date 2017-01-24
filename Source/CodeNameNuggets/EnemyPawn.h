// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "EnemyPawn.generated.h"

UCLASS()
class CODENAMENUGGETS_API AEnemyPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* EnemyMesh;

public:
	// Sets default values for this pawn's properties
	AEnemyPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// called when Pawn has been hit
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(Category = target, BlueprintReadOnly)
	class AActor* currentTarget;

private:

	FRotator enemyAttitude;

	float CurrentAirSpeed;

	void FlyInCircle();

	void FireControl();

	void AttackTarget(AActor* Target);

	bool CanAttack();

	void resetTimer();

	float currentAttackTimer;

	float AttackInterval;
	
};
