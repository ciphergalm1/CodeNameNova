// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "MissileCustom.h"
#include "CustomExplosion_Aircraft.h"
#include "EnemyPawn.h"
#include "FighterPawn.h"


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
	MissileMeshComponent->SetNotifyRigidBodyCollision(true);

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


	// SetActorEnableCollision(true);
	// set up the missile
	currentAirSpeed = 10000.f;
	bHasHitTarget = false;
	bHasBeenFired = false;
	bHasTarget = false;

	// set up homing ability
	homingAngle = 3.f;

	// set up missile damage
	damage = 35.f;

	SelfDestructionTimer = 10.0f;
	
	fLifeTimeMax = 25.0f;

	currentTarget = nullptr;
	turnRate = 80.f;
}

// Called when the game starts or when spawned
void AMissileCustom::BeginPlay()
{
	Super::BeginPlay();

	//currentTarget = ;
	fLifeTime = 0.f;
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

		if (bHasTarget&&!bHasHitTarget) {
			Homing(currentTarget);
		}
		else {
			bStartDestruction = true;
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

void AMissileCustom::EngageTarget(AActor * target, APawn* newMissileOwner)
{
	// set the target
	SetTarget(target);
	// set the owner of the missile
	MissileOwner = newMissileOwner;
	//fire the missile
	Fire();
}

void AMissileCustom::SetDamage(float val)
{
	damage = val;
}

bool AMissileCustom::hasTarget()
{
	return bHasTarget;
}

void AMissileCustom::Boosting()
{
	FVector movment = FVector(currentAirSpeed, 0.f, 0.f);
	AddActorLocalOffset(movment*GetWorld()->GetDeltaSeconds());
}

void AMissileCustom::SetTarget(AActor * target)
{
	if (target != nullptr) {
		currentTarget = target;
		bHasTarget = true;
	}
}

void AMissileCustom::Homing(AActor * target)
{
	// add cone detection
	if (target) {
		FVector currentLocation = GetActorLocation();
		FVector targetLocation = target->GetActorLocation();
		FVector targetVector = targetLocation - currentLocation;
		float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(MissileMeshComponent->GetForwardVector(), targetVector)));
		if (Angle > homingAngle) {
			bHasTarget = false;
		}

		if (bHasTarget) {
			FVector NewVector = FMath::VInterpNormalRotationTo(GetActorForwardVector(), targetVector, GetWorld()->GetDeltaSeconds(), turnRate);
			SetActorRotation(NewVector.Rotation());
		}
	}
	
}

void AMissileCustom::Fire()
{
	bHasBeenFired = true;
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
	if (currentTarget == Other) {
		// The missile hit its target

		//FString messageOwner = "Missile Owner: " + MissileOwner->GetName();
		//FString messagehit = " Hit Target: " + Other->GetName();
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, messageOwner);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, messagehit);

		if (Other->GetClass()->IsChildOf(AEnemyPawn::StaticClass())) {
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Other actor is EnemyPawn type"));
			AEnemyPawn* tempTarget = Cast<AEnemyPawn>(Other);
			tempTarget->ReceiveDamage(damage);
		}

		if (Other->GetClass()->IsChildOf(AFighterPawn::StaticClass())) {
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Other actor is EnemyPawn type"));
			AFighterPawn* tempTarget = Cast<AFighterPawn>(Other);
			tempTarget->ReceiveDamage(damage);
		}



		SpawnExplosion();
		SelfDestruction();
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString("Target hit!"));

		bHasHitTarget = true;
	}
	else if ( Other != MissileOwner ) {
		// the missile hit some thing which is not supposed to hit
		SpawnExplosion();
		SelfDestruction();

		bHasHitTarget = true;
	}
	else {
		// handling error: hit other missile or yourself

		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Missile is not working correctly");
		FString messageOwner = "Missile owner: " + MissileOwner->GetName();
		FString messagehit = " Hit: " + Other->GetName();
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, messageOwner);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, messagehit);
		//SpawnExplosion();
		//SelfDestruction();
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString("Target hit!"));

		//bHasHitTarget = true;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Something went wrong on AMissileCustom::NotifyHit"));
	}
}