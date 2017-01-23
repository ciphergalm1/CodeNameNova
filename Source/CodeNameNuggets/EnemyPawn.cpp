// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "EnemyPawn.h"
#include "MissileCustom.h"

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
	
	EnemyMesh->SetEnableGravity(false);

	// set up enemy aircraft movment
	CurrentAirSpeed = 6000.f;
}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
	currentTarget = GetWorld()->GetFirstPlayerController()->GetPawn();
}

// Called every frame
void AEnemyPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	// manage attack timer;
	currentAttackTimer += GetWorld()->GetDeltaSeconds();
	FlyInCircle();
	FireControl();
}

// Called to bind functionality to input
void AEnemyPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

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
	FVector SpawnLocation = GetActorLocation() + EnemyMesh->GetSocketLocation(FName("Pylon_Main"));
	FRotator SpawnRotation = GetActorRotation();
	AMissileCustom* missile = GetWorld()->SpawnActor<AMissileCustom>(SpawnLocation, SpawnRotation, SpawnParams);
	missile->EngageTarget(Target, GetName());
}

bool AEnemyPawn::CanAttack()
{
	bool result = false;
	result = (currentAttackTimer < AttackInterval) ? true : false;
	return result;
}

void AEnemyPawn::resetTimer()
{
	currentAttackTimer = 0.f;
}

