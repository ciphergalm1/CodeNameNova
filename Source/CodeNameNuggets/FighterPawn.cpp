// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "FighterPawn.h"
#include "CustomExplosion_Aircraft.h"
#include "MissileCustom.h"

AFighterPawn::AFighterPawn()
{
	CurrentPlayerController = GetWorld()->GetFirstPlayerController();

	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		// change plane mesh to custom fighter aircraft mesh
		FConstructorStatics()
			: PlaneMesh(TEXT("StaticMesh'/Game/Assets/Models/MiG-21/MiG-21.MiG-21'"))
		{
		}

	};
	static FConstructorStatics ConstructorStatics;

	/*
	static ConstructorHelpers::FObjectFinder<UBlueprint> ExplosionBase(TEXT("Blueprint'/Game/Assets/ParticleSystem/Blueprint_Effect_CustomExplosion.Blueprint_Effect_CustomExplosion'"));
	if (ExplosionBase.Object) {
		Explosion = (UClass*)ExplosionBase.Object->GeneratedClass;
	}
	*/

	// Create static mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	//add tag to Player PlaneMesh
	PlaneMesh->ComponentTags.Push(FName("PlayerAircraft"));
	RootComponent = PlaneMesh;
	PlaneMesh->SetSimulatePhysics(true);

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 1000.0f;		// The camera follows at this distance behind the character	
	SpringArm->SocketOffset = FVector(0.f, 0.f, 300.f);      // Adding socket to the spring arm ( for camera offset)
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 5.f;
	

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	// Get Engine Sound Ref
	static ConstructorHelpers::FObjectFinder<USoundCue> EngineSoundRef(
		TEXT("SoundCue'/Game/Assets/SFX/EngineSound/MiG-21_ENG.MiG-21_ENG'")
	);

	EngineSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineSoundObj"));
	EngineSoundComponent->SetupAttachment(RootComponent);
	
	// looping sound problem not solved........
	// checking looping sound problem
	if (EngineSoundRef.Succeeded()) {
		//EngineSound->VolumeMultiplier = .3f;
		EngineSoundComponent->SetSound(EngineSoundRef.Object);
		EngineSoundComponent->SetVolumeMultiplier(.5f);
		EngineSoundComponent->bStopWhenOwnerDestroyed = true;
		EngineSoundComponent->bAlwaysPlay = false;
		EngineSoundComponent->Activate();
	}

	// setting up the afterburner component
	ConstructorHelpers::FObjectFinder<UParticleSystem> afterBurnerRef(TEXT("ParticleSystem'/Game/Assets/ParticleSystem/P_AfterBurner.P_AfterBurner'"));
	AfterBurnerComponent = CreateDefaultSubobject<UParticleSystemComponent>("Afterburner component");
	if (afterBurnerRef.Succeeded()) {
		AfterBurnerComponent->SetTemplate(afterBurnerRef.Object);
		AfterBurnerComponent->SetupAttachment(RootComponent,FName("AfterBurner"));
	}

	// setting up the engine viberation
	


	// Setting aircraft parameters
	isAlive = true;
	Acceleration = 400.f;
	TurnSpeed = 80.f;
	MaxSpeed = 8000.f;
	MinSpeed = 3000.f;
	CurrentForwardSpeed = 1000.f;
	CurrentYawSpeed = 0.0f;
	aircraftHP = 100.f;
	SpeedDelta = MaxSpeed - MinSpeed;
	NormalAirSpeed = (MinSpeed + MaxSpeed) / 2;
	bMissileOnLeftPylon = true;
}

void AFighterPawn::Tick(float DeltaSeconds)
{


	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);

	// check component tag
	/*
	if (PlaneMesh->ComponentHasTag("PlayerAircraft")&&this->ActorHasTag("PlayerAircraft")) {
		FString tagMessage = this->Tags.Top().ToString();
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, tagMessage);
	}
	*/

	// Move plane forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0, 0, 0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation);

	// config afterburnerEffect
	ConfigAfterBurner();

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
}

void AFighterPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("You plane hit something!"));
	// Destroy the pawn if player hit the ground
	if (Other->ActorHasTag("Terrain")) {
		// emit the explosion
		SpawnExplosion();
		EngineSoundComponent->SetVolumeMultiplier(0.0f);
		//EngineSoundComponent->SetPaused(true);
		//EngineSoundComponent->PlaybackCompleted(EngineSoundComponent->GetAudioComponentID(),false);
		EngineSoundComponent->FadeOut(.5f, .0f);
		EngineSoundComponent->Stop();
		EngineSoundComponent->Deactivate();
		//EngineSoundComponent->DestroyComponent();
		//PlaneMesh->DestroyComponent();
		Destroy();
	}

	if (Other->ActorHasTag("Aircraft")) {
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("You hit an C-5!"));
		ReceiveDamage(10.0f);
	}
	
	FRotator CurrentRotation = GetActorRotation(RootComponent);
	SetActorRotation(FQuat::Slerp(CurrentRotation.Quaternion(), HitNormal.ToOrientationQuat(), 0.025f));
	
	
}


void AFighterPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// Bind our control axis' to callback functions
	PlayerInputComponent->BindAxis("Thrust", this, &AFighterPawn::ThrustInput);
	PlayerInputComponent->BindAxis("MoveUp", this, &AFighterPawn::MoveUpInput);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFighterPawn::MoveRightInput);
	PlayerInputComponent->BindAxis("MoveYaw", this, &AFighterPawn::MoveYawInput);
	PlayerInputComponent->BindAction("FireMissile", IE_Pressed, this, &AFighterPawn::FireMissile);
	PlayerInputComponent->BindAction("FireGuns", IE_Pressed, this, &AFighterPawn::FireGuns);
}

/** manage aircraft thrust input*/
void AFighterPawn::ThrustInput(float Val)
{
	// Is there no input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	float NewForwardSpeed = 0.0f;
	if (bHasInput) {
		float CurrentAcc = Val * Acceleration;
		NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
		//ReceiveDamage(Val*(GetWorld()->GetDeltaSeconds()));
	}
	else {
		NewForwardSpeed = FMath::FInterpTo(CurrentForwardSpeed, NormalAirSpeed, GetWorld()->GetDeltaSeconds(), 1.0f);
	}
	/*
	// If input is not held down, reduce speed
	float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	*/

	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);

	CurrentThrustRatio = (CurrentForwardSpeed - MinSpeed )/ SpeedDelta;
	ConfigEngineSound();


	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(CurrentThrustRatio));
	FVector gVector = FVector(0,0,-9.8);
	if(CurrentThrustRatio<.4)
	PlaneMesh->AddForce(gVector);

}

void AFighterPawn::MoveUpInput(float Val)
{
	// Target pitch speed is based in input
	float TargetPitchSpeed = (Val * TurnSpeed * -1.f);

	// When steering, we decrease pitch slightly
	TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

	// Smoothly interpolate to target pitch speed
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AFighterPawn::MoveRightInput(float Val)
{
	//	disable yaw on the fighter aircraft
	// Target yaw speed is based on input
	//float TargetYawSpeed = (Val * TurnSpeed);

	// Is there any left/right input?
	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If turning, yaw value is used to influence roll
	// If not turning, roll to reverse current roll value
	float TargetRollSpeed = Val*TurnSpeed;

	// Smoothly interpolate roll speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AFighterPawn::MoveYawInput(float Val)
{
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	// If input is not held down, reduce speed
	float TargetYawSpeed = bHasInput ? (Val * TurnSpeed * -1.f) : 0;
	// Target pitch speed is based in input

	// When steering, we decrease pitch slightly
	//TargetYawSpeed += (FMath::Abs(CurrentForwardSpeed) * -0.01f);

	// Smoothly interpolate to target pitch speed
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AFighterPawn::FireMissile() {
	// define fire missile function
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Player have fired!"));
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	FRotator SpawnRotation = GetActorRotation();
	FVector SpawnLocation;
	if (bMissileOnLeftPylon) {
		SpawnLocation = PlaneMesh->GetSocketLocation(FName("Pylon_L"));
	}
	else {
		SpawnLocation = PlaneMesh->GetSocketLocation(FName("Pylon_R"));
	}
	bMissileOnLeftPylon = !bMissileOnLeftPylon;
	AMissileCustom* missile = GetWorld()->SpawnActor<AMissileCustom>(SpawnLocation, SpawnRotation, SpawnParams);
	//missile->EngageTarget(Target);
}

void AFighterPawn::FireGuns() {
	// define fire guns function

}

float AFighterPawn::GetAirSpeed() const
{
	return CurrentForwardSpeed/10;
}

int AFighterPawn::GetAltitude() const
{
	FVector currentLocation = GetActorLocation();
	float result = currentLocation.Z / 100;
	return FMath::FloorToInt(result);
}

float AFighterPawn::GetThrust() const
{
	return CurrentThrustRatio;
}

int AFighterPawn::GetBearing() const
{
	FRotator currentRotation = GetActorRotation();
	// get the yaw set
	float result = currentRotation.Yaw;
	if (result < 0) {
		result += 360.0f;
	}
	return FMath::FloorToInt(result);
}

int AFighterPawn::GetHealth() const
{
	return FMath::FloorToInt(aircraftHP);
}

void AFighterPawn::SetSoundVolume(float volume)
{
	EngineSoundComponent->SetVolumeMultiplier(volume);
}

void AFighterPawn::SetSoundRefNull()
{
	EngineSoundComponent->SetSound(NULL);
}

void AFighterPawn::ReceiveDamage(float damageVal)
{
	FString hitMessage = "You got hit by :";
	hitMessage += FString::SanitizeFloat(damageVal);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, hitMessage);
	aircraftHP -= damageVal;
}

void AFighterPawn::SpawnExplosion()
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
		world->SpawnActor<ACustomExplosion_Aircraft>(SpawnLocation,SpawnRotation,SpawnParams);
	}
	
}

void AFighterPawn::ConfigEngineSound()
{
	if (isAlive) {
		float audioPitch = CurrentThrustRatio*0.6 + 0.8;
		EngineSoundComponent->SetPitchMultiplier(audioPitch);
	}
}

void AFighterPawn::ConfigEngineViberation()
{
}

void AFighterPawn::ConfigAfterBurner()
{
	if (isAlive) {

		// controll the engine effect
		float outputRatio = CurrentThrustRatio * 5.0f;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(outputRatio));
		outputRatio = FMath::Clamp<float>(outputRatio, 0.1f, 5.0f);
		float distortionRatio = CurrentThrustRatio * 8.0f;
		distortionRatio = FMath::Clamp<float>(distortionRatio, 0.1f, 8.0f);
		AfterBurnerComponent->SetFloatParameter(FName("AfterBurnerRatio"), outputRatio);
		AfterBurnerComponent->SetFloatParameter(FName("DistortionLifeTime"), distortionRatio);
	}
}
