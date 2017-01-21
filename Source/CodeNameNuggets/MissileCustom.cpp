// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "MissileCustom.h"
#include "CustomExplosion_Aircraft.h"


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
	MissileMeshComponent->SetEnableGravity(false);
	MissileMeshComponent->SetMobility(EComponentMobility::Movable);

	ConstructorHelpers::FObjectFinder<UParticleSystem> missileTrailRef(TEXT("ParticleSystem'/Game/Assets/ParticleSystem/P_MissileTrail.P_MissileTrail'"));
	MissileTrailComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Missile Trail"));
	if (missileTrailRef.Succeeded()) {
		MissileTrailComponent->SetTemplate(missileTrailRef.Object);
	}

	MissileTrailComponent->SetupAttachment(RootComponent, FName("tail"));
	MissileTrailComponent->bAutoActivate = true;

	// set up missile sound
	ConstructorHelpers::FObjectFinder<USoundCue> missileSoundRef(TEXT("SoundCue'/Game/SFX/MissileSound/MissileEngine_Cue.MissileEngine_Cue'"));
	MissileSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Missile Sound"));
	if (missileSoundRef.Succeeded()) {
		MissileSoundComponent->SetSound(missileSoundRef.Object);
	}
	MissileSoundComponent->SetupAttachment(RootComponent, FName("tail"));
	MissileSoundComponent->bAutoActivate = true;
	MissileSoundComponent->Play();

	// set up the missile
	currentAirSpeed = 8000.f;
	bHasHitTarget = false;
	bHasBeenFired = false;

	SelfDestructionTimer = 3.0f;
	
	fLifeTimeMax = 25.0f;

	currentTarget = nullptr;
	turnRate = 80.f;
}

// Called when the game starts or when spawned
void AMissileCustom::BeginPlay()
{
	Super::BeginPlay();
	
	//currentTarget = ;
	AActor* Target = nullptr;
	fLifeTime = 0.f;
	if (Target!=nullptr) {
		EngageTarget(Target, GetName());
	}
	else {
		Target = GetWorld()->GetFirstPlayerController()->GetPawn();
	}
	EngageTarget(Target, GetName());
	FString message = "Target Accquired : " + currentTarget->GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, message);
	//Fire();
}

// Called every frame
void AMissileCustom::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	fLifeTime += GetWorld()->GetDeltaSeconds();
	if (fLifeTime > fLifeTimeMax) {
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString("Missile running out of fuel!"));
		SpawnExplosion();
		Destroy();
	}

	if (bHasBeenFired) {
		Boosting();

		if (!bHasHitTarget) {
			Homing(currentTarget);
		}

		if (bStartDestruction) {
			SelfDestructionTimer -= GetWorld()->GetDeltaSeconds();
			if (SelfDestructionTimer<0) {
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString("Missile is gone!"));
				Destroy();
			}
		}
	}
}

void AMissileCustom::EngageTarget(AActor * target, FString newMissileOwner)
{
	// set the target
	SetTarget(target);
	// set the owner of the missile
	MissileOwner = newMissileOwner;
	//fire the missile
	Fire();
}

void AMissileCustom::Boosting()
{
	FVector movment = FVector(currentAirSpeed, 0.f, 0.f);
	AddActorLocalOffset(movment*GetWorld()->GetDeltaSeconds());
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
	//FVector movment = FVector(currentAirSpeed, 0.f, 0.f);
	//AddActorLocalOffset(movment);
}

void AMissileCustom::SelfDestruction()
{
	MissileMeshComponent->DestroyComponent();
	SelfDestructionTimer -= GetWorld()->GetDeltaSeconds();
	if (SelfDestructionTimer<0) {
		Destroy();
	}
}

void AMissileCustom::SpawnExplosion()
{
	UWorld* const world = GetWorld();
	if (world) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();
		world->SpawnActor<ACustomExplosion_Aircraft>(SpawnLocation, SpawnRotation, SpawnParams);
	}
}

void AMissileCustom::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (MissileOwner.Equals(Other->GetName())) {
		
		FString messageOwner = "Owner: " + MissileOwner;
		FString messagehit = "Hit: " + Other->GetName();
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, messageOwner);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, messagehit);
	
		//check world
		SpawnExplosion();
		SelfDestruction();
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString("Target hit!"));
		
		bHasHitTarget = true;
	}
	

	
}
