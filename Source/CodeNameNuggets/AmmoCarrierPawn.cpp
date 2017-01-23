// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "AmmoCarrierPawn.h"


// Sets default values
AAmmoCarrierPawn::AAmmoCarrierPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAmmoCarrierPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAmmoCarrierPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AAmmoCarrierPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

