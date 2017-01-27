// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "EnemyPawn.h"
#include "MissileCustom.h"
#include "CustomExplosion_Aircraft.h"

// Sets default values
AEnemyPawn::AEnemyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UStaticMesh> enemyMeshRef(TEXT("StaticMesh'/Game/Assets/Models/F16/F-16C.F-16C'"));
	EnemyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Enemy Mesh"));
	if (enemyMeshRef.Succeeded()) {
		EnemyMesh->SetStaticMesh(enemyMeshRef.Object);
	}
	EnemyMesh->ComponentTags.Push(FName("EnemyAircraft"));
	Tags.Push(FName("EnemyAircraft"));
	Tags.Insert(FName("EnemyAircraft"), 0);
	
	RootComponent = EnemyMesh;
	EnemyMesh->SetSimulatePhysics(true);
	EnemyMesh->bAutoActivate = true;
	EnemyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	EnemyMesh->SetComponentTickEnabled(true);
	EnemyMesh->SetNotifyRigidBodyCollision(true);
	EnemyMesh->SetEnableGravity(false);

	// set up enemy aircraft movment
	CurrentAirSpeed = 6000.f;

	// set up lock on status
	LockOnStatus = 0;

	currentAttackTimer = 0.f;
	Health = 100.f;
	AttackInterval = 15.f;
}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
	currentTarget = GetWorld()->GetFirstPlayerController()->GetPawn();
}

void AEnemyPawn::NotifyHit(UPrimitiveComponent * MyComp, AActor * Other, UPrimitiveComponent * OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	FString message = "This is your enemy fighter pawn: " + GetName();
	message += ". I have been hit by";
	message += Other->GetName();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, message);
}

// Called every frame
void AEnemyPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (IsAlive()) {
		// pawn is alive , continue the attack
		// manage attack timer;
		currentAttackTimer += GetWorld()->GetDeltaSeconds();
		FlyInCircle();
		//FireControl();
	}
	else {
		// pawn is dead
		SelfDestruction();
	}
}

// Called to bind functionality to input
void AEnemyPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void AEnemyPawn::ReceiveDamage(float val)
{
	Health -= val;
}

void AEnemyPawn::SelfDestruction()
{
	// spawn explosion 
	UWorld* const world = GetWorld();
	if (world) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();
		world->SpawnActor<ACustomExplosion_Aircraft>(SpawnLocation, SpawnRotation, SpawnParams);
	}
	// destroy the enemy
	EnemyMesh->DestroyComponent();
	Destroy();
}

bool AEnemyPawn::IsAlive()
{
	bool result = true;
	if (Health <= 0) {
		result = false;
	}
	return result;
}

void AEnemyPawn::SetLockOnStatus(int val)
{
	LockOnStatus = FMath::Clamp(val, 0, 2);
	FString message = "Lock on status: " + LockOnStatus;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, message);
}

int AEnemyPawn::GetLockOnStatus()
{
	return LockOnStatus;
}

void AEnemyPawn::FlyInCircle()
{
	FVector movingOffset = FVector(CurrentAirSpeed, 0.f, 0.f);
	AddActorLocalOffset(movingOffset*GetWorld()->GetDeltaSeconds());

	enemyAttitude = FRotator(20.f, 0.f, 0.f);
	AddActorLocalRotation(enemyAttitude*GetWorld()->GetDeltaSeconds());
}

void AEnemyPawn::FireControl()
{
	if (CanAttack()) {
		AttackTarget(currentTarget);
		resetTimer();
	}
}

void AEnemyPawn::AttackTarget(AActor * Target)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("I have fire at you!"));
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	FVector SpawnLocation = EnemyMesh->GetSocketLocation(FName("Pylon_Main"));
	FRotator SpawnRotation = GetActorRotation();
	AMissileCustom* missile = GetWorld()->SpawnActor<AMissileCustom>(SpawnLocation, SpawnRotation, SpawnParams);
	missile->EngageTarget(Target, this);
}

bool AEnemyPawn::CanAttack()
{
	bool result = false;
	result = (currentAttackTimer > AttackInterval) ? true : false;
	return result;
}

void AEnemyPawn::resetTimer()
{
	currentAttackTimer = 0.f;
}

