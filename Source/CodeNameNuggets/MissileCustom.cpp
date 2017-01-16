// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "MissileCustom.h"


// Sets default values
AMissileCustom::AMissileCustom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UStaticMesh> missileMeshRef(TEXT("StaticMesh'/Game/Assets/Models/AIM_9/AIM_009.AIM_009'"));
	MissileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Missile Mesh"));
	if (missileMeshRef.Succeeded()) {
		MissileMeshComponent->SetStaticMesh(missileMeshRef.Object);
	}
	RootComponent = MissileMeshComponent;
	MissileMeshComponent->SetSimulatePhysics(true);
	MissileMeshComponent->SetMobility(EComponentMobility::Movable);

	ConstructorHelpers::FObjectFinder<UParticleSystem> missileTrailRef(TEXT("ParticleSystem'/Game/Assets/ParticleSystem/P_MissileTrail.P_MissileTrail'"));
	MissileTrailComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Missile Trail"));
	if (missileTrailRef.Succeeded()) {
		MissileTrailComponent->SetTemplate(missileTrailRef.Object);
	}

	MissileTrailComponent->SetupAttachment(RootComponent, FName("tail"));
	MissileTrailComponent->bAutoActivate = true;

	// set up the missile
	currentAirSpeed = 8000.f;
	bHasHitTarget = false;
	bHasBeenFired = false;

	currentTarget = nullptr;
	turnRate = 20.f;
}

// Called when the game starts or when spawned
void AMissileCustom::BeginPlay()
{
	Super::BeginPlay();
	
	currentTarget = GetWorld()->GetFirstPlayerController()->GetPawn();
	Fire();
}

// Called every frame
void AMissileCustom::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (bHasBeenFired&&(!bHasHitTarget)) {
		Homing(currentTarget);
	}
}

void AMissileCustom::EngageTarget(AActor * target)
{
	//set the target
	SetTarget(target);
	//fire the missile
	Fire();
}

void AMissileCustom::SetTarget(AActor * target)
{
	currentTarget = target;
}

void AMissileCustom::Homing(AActor * target)
{
	FVector currentLocation = GetActorLocation();
	FVector targetLocation = target->GetActorLocation();
	FVector targetVector = targetLocation - currentLocation;
	FVector NewVector = FMath::VInterpNormalRotationTo(GetActorForwardVector(),targetVector,GetWorld()->GetDeltaSeconds(),turnRate);
	SetActorRotation(NewVector.Rotation());
}

void AMissileCustom::Fire()
{
	bHasBeenFired = true;
	FVector movingTarget = FVector(currentAirSpeed, 0.f, 0.f);
	AddActorLocalOffset(movingTarget);
}

