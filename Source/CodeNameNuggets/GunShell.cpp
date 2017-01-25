// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "GunShell.h"


// Sets default values
AGunShell::AGunShell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Shell = CreateDefaultSubobject<USphereComponent>(TEXT("Shell"));
	RootComponent = Shell;
	Shell->SetNotifyRigidBodyCollision(true);

	ConstructorHelpers::FObjectFinder<UParticleSystem> trailRef(TEXT("ParticleSystem'/Game/Assets/ParticleSystem/ShellTrail.ShellTrail'"));
	ShellTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Shell Trail"));
	if (trailRef.Succeeded()) {
		ShellTrail->SetTemplate(trailRef.Object);
	}

	ShellSpeed = 9000.f;
}

// Called when the game starts or when spawned
void AGunShell::BeginPlay()
{
	Super::BeginPlay();
	ShellTrail->ActivateSystem();
}

// Called every frame
void AGunShell::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	shellTravel();
}

void AGunShell::shellTravel()
{
	FVector move = ShellSpeed * FVector(1.0f, 0.f, 0.f);
	AddActorLocalOffset(move);
}

void AGunShell::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString("Shell hit!"));
}



