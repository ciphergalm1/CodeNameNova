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

	void ReceiveDamage(float val);

	void SelfDestruction();

	bool IsAlive();

	int LockOnStatus; // 0 for idle , 1 for selected , 2 for locked on

	void SetLockOnStatus(int val);

	UFUNCTION(Category = LockOnStatus, BlueprintCallable)
	int GetLockOnStatus();

private:

	FRotator enemyAttitude;

	float CurrentAirSpeed;

	void FlyInCircle();

	void FlyStraight();

	void FireControl();

	void AttackTarget(AActor* Target);

	bool CanAttack();

	void resetTimer();

	float currentAttackTimer;

	float AttackInterval;

	float Health;

	float point;
	
};
