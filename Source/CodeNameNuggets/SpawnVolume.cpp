// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "SpawnVolume.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemyPawn.h"


// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// create the box component
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	RootComponent = SpawnVolume;

	// set up spawn delay
	MinSpawnDelay = 15.f;
	MaxSpawnDelay = 30.f;


}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	//DelayedSpawn();
}
	

// Called every frame
void ASpawnVolume::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

FVector ASpawnVolume::GetRandomPoint()
{
	FVector SpawnOrigin = SpawnVolume->Bounds.Origin;
	FVector SpawnExtent = SpawnVolume->Bounds.BoxExtent;

	return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
}

void ASpawnVolume::SpawnEnemy()
{
	if (EnemyToSpawn != NULL) {
		UWorld* const world = GetWorld();
		if (world) {
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			FVector SpawnLocation = GetRandomPoint();
			FRotator SpawnRotation = GetActorRotation();

			AEnemyPawn* newEnemy = world->SpawnActor<AEnemyPawn>(SpawnLocation, SpawnRotation, SpawnParams);
			//DelayedSpawn();
		}
	}

}

void ASpawnVolume::DelayedSpawn()
{
	SpawnDelay = FMath::FRandRange(MinSpawnDelay, MaxSpawnDelay);
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawnVolume::SpawnEnemy, SpawnDelay, false);
}

