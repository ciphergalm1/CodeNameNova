// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "MissileCustom.h"


// Sets default values
AMissileCustom::AMissileCustom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMissileCustom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMissileCustom::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

