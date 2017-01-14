// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "CustomExplosion_Aircraft.h"


// Sets default values
ACustomExplosion_Aircraft::ACustomExplosion_Aircraft()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACustomExplosion_Aircraft::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACustomExplosion_Aircraft::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

