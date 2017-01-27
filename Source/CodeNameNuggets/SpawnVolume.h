// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class CODENAMENUGGETS_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Function for open access to spawn volume
	FORCEINLINE class UBoxComponent* GetSpawnVolume() const { return SpawnVolume; }

	// get random point in the volume
	UFUNCTION(BlueprintPure, Category = Spawning)
	FVector GetRandomPoint();

	void SpawnEnemy();

	// set minimun spawn delay
	UPROPERTY(Category = Spawning, EditAnywhere, BlueprintReadWrite)
	float MinSpawnDelay;

	// set maximum spawn delay
	UPROPERTY(Category = Spawning, EditAnywhere, BlueprintReadWrite)
	float MaxSpawnDelay;

	void DelayedSpawn();

	float SpawnDelay;

protected:
	// the pawn to spawn
	UPROPERTY(Category = Spawning, EditAnywhere)
	TSubclassOf<class AEnemyPawn> EnemyToSpawn;

	FTimerHandle SpawnTimerHandle;

private:
	// Box component where enemy plane is spawned
	UPROPERTY(Category = SpawningVolume, VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true"))
	class UBoxComponent* SpawnVolume;
	
	
};
