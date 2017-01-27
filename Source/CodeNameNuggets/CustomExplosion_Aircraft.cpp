// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "CustomExplosion_Aircraft.h"


// Sets default values
ACustomExplosion_Aircraft::ACustomExplosion_Aircraft()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// initialize the particle system component
	ConstructorHelpers::FObjectFinder<UParticleSystem> explosionRef(TEXT("ParticleSystem'/Game/Assets/ParticleSystem/P_Explosion.P_Explosion'"));
	ExplosionEffectComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ExplosionParticleEffect"));
	if (explosionRef.Succeeded()) {
		ExplosionEffectComponent->SetTemplate(explosionRef.Object);
	}

	//ExplosionEffectComponent->bAutoDestroy = true;
	ExplosionEffectComponent->bAutoActivate = true;
	FVector scaling = 8.0f * FVector(1.0f, 1.0f, 1.0f);
	ExplosionEffectComponent->SetRelativeScale3D(scaling);
	RootComponent = ExplosionEffectComponent;

	// initialize the explosion sound component
	ConstructorHelpers::FObjectFinder<USoundCue> soundRef(TEXT("SoundCue'/Game/SFX/Explosion/Explosion_Cue.Explosion_Cue'"));
	ExplosionSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ExplosionSound"));
	if (soundRef.Succeeded()) {
		ExplosionSoundComponent->SetSound(soundRef.Object);
		//explosionCue = soundRef.Object;
	}
	ExplosionSoundComponent->SetupAttachment(RootComponent);
	

	fMaxLifeTime = 4.0f;
	fLifeTime = 0.0f;
}

// Called when the game starts or when spawned
void ACustomExplosion_Aircraft::BeginPlay()
{
	Super::BeginPlay();
	ExplosionEffectComponent->ActivateSystem();
	ExplosionSoundComponent->Play();
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString("Begin play method"));
	//ExplosionSoundComponent->Activate();
	
	/*
	if (explosionCue) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), explosionCue, GetActorLocation(), .8f, 1.0f);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString("Play sound method"));
		fLifeTime = 0.0f;
	}
	*/
}

// Called every frame
void ACustomExplosion_Aircraft::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	fLifeTime += 1.0*(GetWorld()->GetDeltaSeconds());
	if (!bIsStillAlive()) {
		Destroy();
	}
}

void ACustomExplosion_Aircraft::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//GetWorld()->GetTimerManager().ClearTimer(explosionHandle);
}

bool ACustomExplosion_Aircraft::bIsStillAlive()
{
	bool result = true;
	result = (fLifeTime<fMaxLifeTime) ? true:false ;
	return result;
}



