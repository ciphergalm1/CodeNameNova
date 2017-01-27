// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "GunShell.h"
#include "CustomExplosion_Aircraft.h"


// Sets default values
AGunShell::AGunShell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Shell = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shell Mesh"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> cannonRef(TEXT("StaticMesh'/Game/Assets/Models/Cannon/Shell.Shell'"));
	if (cannonRef.Succeeded()) {
		Shell->SetStaticMesh(cannonRef.Object);
	}
	RootComponent = Shell;
	Shell->SetSimulatePhysics(true);
	Shell->SetEnableGravity(false);
	Shell->SetMobility(EComponentMobility::Movable);
	Shell->SetNotifyRigidBodyCollision(true);

	ConstructorHelpers::FObjectFinder<UParticleSystem> trailRef(TEXT("ParticleSystem'/Game/Assets/ParticleSystem/P_BulletTrail.P_BulletTrail'"));
	ShellTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Shell Trail"));
	if (trailRef.Succeeded()) {
		ShellTrail->SetTemplate(trailRef.Object);
	}
	FVector trailScale = 10.f * FVector(1.0f, 1.0f, 1.0f);
	ShellTrail->SetWorldScale3D(trailScale);
	ShellTrail->SetupAttachment(RootComponent);

	ShellSpeed = 48000.f;
}

// Called when the game starts or when spawned
void AGunShell::BeginPlay()
{
	Super::BeginPlay();
	ShellTrail->ActivateSystem();
	FVector endPos = GetActorLocation() + 50000.f * GetActorForwardVector();
	DrawDebugLine(GetWorld(), GetActorLocation(), endPos, FColor::Blue, false, 3.f);
}

// Called every frame
void AGunShell::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	shellTravel();
}

void AGunShell::shellTravel()
{
	FVector move = FVector(ShellSpeed, 0, 0);
	AddActorLocalOffset(move*GetWorld()->GetDeltaSeconds());
}

void AGunShell::SpawnExplosion()
{
	// check spawn object
	UWorld* const world = GetWorld();
	//check world
	if (world) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();
		world->SpawnActor<ACustomExplosion_Aircraft>(SpawnLocation, SpawnRotation, SpawnParams);
	}
}

void AGunShell::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	FString message = "Shell hit " + Other->GetName();
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, message);


	SpawnExplosion();
	Shell->DestroyComponent();
	Destroy();
}



