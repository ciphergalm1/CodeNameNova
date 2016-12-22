// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "EarthRotator.h"


// Sets default values
AEarthRotator::AEarthRotator()
{
	// import mesh for the earth

	// load the earth mesh for only once;
	struct FConstructorStatics 
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> EarthMesh;
		FConstructorStatics() : EarthMesh(TEXT("/Game/Assets/Models/Earth/Earth")) 
		{}
	};
	
	struct FConstructorStatics ConstructorStatics;

	//create static mesh component
	EarthMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EarthMesh0"));
	EarthMesh->SetStaticMesh(ConstructorStatics.EarthMesh.Get());
	RootComponent = EarthMesh;

	// set rotate speed;
	RotateSpeed = -0.4f;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEarthRotator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEarthRotator::Tick( float DeltaTime )
{
	Rotate(DeltaTime);

	Super::Tick( DeltaTime );
}

void AEarthRotator::Rotate(float DeltaSeconds)
{
	FRotator RotationDelta = FRotator(0,0,0);
	RotationDelta.Yaw = RotateSpeed * DeltaSeconds;
	AddActorLocalRotation(RotationDelta);
}

