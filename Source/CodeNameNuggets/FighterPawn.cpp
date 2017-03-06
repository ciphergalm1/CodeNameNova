// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "FighterPawn.h"
#include "CustomExplosion_Aircraft.h"
#include "MissileCustom.h"
#include "GunShell.h"
#include "EnemyPawn.h"
#include "MyPlayerState.h"
#include "MyGameStateBase.h"

AFighterPawn::AFighterPawn()
{
	//CurrentPlayerController = GetWorld()->GetFirstPlayerController();

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
	ConstructorHelpers::FObjectFinder<USoundCue> EngineSoundRef(TEXT("SoundCue'/Game/SFX/EngineSound/MiG-21_ENG.MiG-21_ENG'"));

	EngineSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineSoundObj"));
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

	EngineSoundComponent->SetupAttachment(RootComponent);
	
	// set up the Gun Sound
	ConstructorHelpers::FObjectFinder<USoundCue> CannonSoundRef(TEXT("SoundCue'/Game/SFX/Cannon/GSh301_Cue.GSh301_Cue'"));
	CannonSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("GunSoundObj"));
	if (CannonSoundRef.Succeeded()) {
		CannonSoundComponent->SetSound(CannonSoundRef.Object);
		CannonSoundComponent->bAutoActivate = false;
		CannonSoundComponent->SetupAttachment(RootComponent);
	}

	// set up the locking sound
	//  SoundCue'/Game/SFX/FireControll/AIM9_Cue.AIM9_Cue'
	ConstructorHelpers::FObjectFinder<USoundCue> LockingSoundRef(TEXT("SoundCue'/Game/SFX/FireControll/BeatTone_Cue.BeatTone_Cue'"));
	LockingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("LockingSoundObject"));
	if (LockingSoundRef.Succeeded()) {
		LockingSoundComponent->SetSound(LockingSoundRef.Object);
		LockingSoundComponent->bAutoActivate = false;
		LockingSoundComponent->SetupAttachment(RootComponent);
	}

	// set up the locked sound 
	// SoundCue'/Game/SFX/FireControll/LockWarning_Cue.LockWarning_Cue'
	ConstructorHelpers::FObjectFinder<USoundCue> LockedSoundRef(TEXT("SoundCue'/Game/SFX/FireControll/LockWarning_Cue.LockWarning_Cue'"));
	LockedSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("LockedSoundObject"));
	if (LockedSoundRef.Succeeded()) {
		LockedSoundComponent->SetSound(LockedSoundRef.Object);
		LockedSoundComponent->bAutoActivate = false;
		LockedSoundComponent->SetupAttachment(RootComponent);
	}

	// set up the missile warning sound
	ConstructorHelpers::FObjectFinder<USoundCue> MissileWarningSoundRef(TEXT("SoundCue'/Game/SFX/FireControll/MissileWarning_Cue.MissileWarning_Cue'"));
	MissileWarningSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MissileWarningSoundObject"));
	if (MissileWarningSoundRef.Succeeded()) {
		MissileWarningSoundComponent->SetSound(MissileWarningSoundRef.Object);
		MissileWarningSoundComponent->bAutoActivate = false;
		MissileWarningSoundComponent->SetupAttachment(RootComponent);
		MissileWarningSoundComponent->Stop();
	}


	// set up the afterburner component
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

	/* set the weapon lock capability */
	DetectionDistance = 60000.f;
	DetectionRadius = 20000.f;
	DetectionShape = FCollisionShape();
	DetectionShape = FCollisionShape::MakeCapsule(DetectionRadius, DetectionDistance);
	//DetectionShape.SetCapsule(3000.f,30000.f);

	// set missile detection shape
	MissileDetectionRadius = 80000.f;
	MissileDetectionShape = FCollisionShape();
	MissileDetectionShape = FCollisionShape::MakeSphere(MissileDetectionRadius);


	/** set up target selection  */
	LockOnGauge = 0;
	CurrentTarget = nullptr;
	targetSelected = nullptr;

	/** set up the missile damage system*/
	MissileDamage = 65.f;

	/** setup cannon*/
	isFiringCannon = false;
	canFireCannon = true;

	aircraftHP = 100.f;
	MissileRemain = 152;

	// set up score
	Score = 0.0f;

	SpeedDelta = MaxSpeed - MinSpeed;
	NormalAirSpeed = (MinSpeed + MaxSpeed) / 2;
	bMissileOnLeftPylon = true;

	// set up player state
	//PlayerState = 

}

void AFighterPawn::Tick(float DeltaSeconds)
{


	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);

	if (IsAlive()) {
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

		// Rotate Camera
		FRotator newCameraRotator(0, 0, 0);
		newCameraRotator.Yaw = CurrentCameraYaw;
		newCameraRotator.Pitch = CurrentCameraPitch;

		SpringArm->RelativeRotation = newCameraRotator;
		/*
		FHitResult hit;
		SpringArm->K2_SetRelativeRotation(newCameraRotator, false, hit, false);
		*/

		// config guns 
		ShootGuns();

		// config target lock on
		LockOnTarget();

		// config afterburnerEffect
		ConfigAfterBurner();

		ToggleMissileWarning();

		// Call any parent class Tick implementation
	}
	else {
		SelfDestruction();
	}
	Super::Tick(DeltaSeconds);
}

void AFighterPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("You plane hit something!"));
	// Destroy the pawn if player hit the ground
	if (Other->ActorHasTag("Terrain")) {
		SelfDestruction();
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
	PlayerInputComponent->BindAxis("CameraUp", this, &AFighterPawn::CameraUpInput);
	PlayerInputComponent->BindAxis("CameraRight", this, &AFighterPawn::CameraRightInput);
	PlayerInputComponent->BindAction("FireMissile", IE_Pressed, this, &AFighterPawn::FireMissile);

	PlayerInputComponent->BindAction("FireGuns", IE_Pressed, this, &AFighterPawn::FireGuns);
	PlayerInputComponent->BindAction("FireGuns", IE_Released, this, &AFighterPawn::StopFireGuns);

	PlayerInputComponent->BindAction("SearchTarget", IE_Pressed, this, &AFighterPawn::SearchTarget);
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
	FVector gVector = 100 * FVector(0,0,-9.8);
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

void AFighterPawn::CameraUpInput(float Val)
{
	// setting new rotation to the spring arm
	// SpringArm
	CurrentCameraPitch = FMath::FInterpTo(CurrentCameraPitch, 180 * Val, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AFighterPawn::CameraRightInput(float Val)
{
	CurrentCameraYaw = FMath::FInterpTo(CurrentCameraYaw, 180 * Val, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AFighterPawn::SearchTarget()
{
	TArray<FHitResult> HitResults;
	FVector StartPos = PlaneMesh->GetSocketLocation(FName("Nose"));
	FVector EndPos = PlaneMesh->GetSocketLocation(FName("Nose")) + GetActorForwardVector()*DetectionDistance;
	FCollisionQueryParams CollisionParams;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("----------------------------------------------------"));
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Start target searching!"));
	FCollisionResponseParams ResponseParams;
	ClearSelectTarget();

	/* Trying debug capsule */
	//FVector center = PlaneMesh->GetSocketLocation(FName("Nose")) + GetActorForwardVector()*DetectDistance;
	//DrawDebugCapsule(GetWorld(), center, 30000.f, 3000.f, PlaneMesh->GetComponentRotation().Quaternion(), FColor::Green, false, 10.f);

	if (targetSelected == nullptr) {
		bool bHasDetecTarget = GetWorld()->SweepMultiByChannel(HitResults, StartPos, EndPos, GetActorForwardVector().ToOrientationQuat(), ECollisionChannel::ECC_Pawn, DetectionShape, CollisionParams, ResponseParams);
		//DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::Green, false, 10.f);
		if (bHasDetecTarget) {
			for (auto it = HitResults.CreateIterator(); it; it++) {
				if ((*it).GetActor() != this && (*it).GetActor()->IsA(AEnemyPawn::StaticClass())) {
					//FVector targetLocation = (*it).Actor->GetActorLocation();
					//FString targetName = "Find target: " + (*it).Component->GetName();
					//DrawDebugSphere(GetWorld(), targetLocation, 500.f, 16, FColor::Green, false, 10.f);
					if ((*it).Actor->GetRootComponent()->ComponentHasTag(FName("EnemyAircraft"))) {
						//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Target Accquired!"));
						//FString message = "Target Accquired: " + (*it).Actor->GetName();
						//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, message);
						AActor* target = (*it).GetActor();
						targetSelected = Cast<AEnemyPawn>(target);
						
					}
				}
			}
			if (targetSelected) {
				//targetSelected = Cast<AEnemyPawn>(CurrentTarget);
				targetSelected->SetLockOnStatus(1);
			}
		}

	}
	/*
	else {
		//  replication of search target
		CurrentTarget = nullptr;
		bool bHasDetecTarget = GetWorld()->SweepMultiByChannel(HitResults, StartPos, EndPos, GetActorRotation().Quaternion(), ECollisionChannel::ECC_Pawn, DetectionShape, CollisionParams, ResponseParams);
		DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::Green, false, 10.f);
		if (bHasDetecTarget) {
			for (auto it = HitResults.CreateIterator(); it; it++) {
				if ((*it).GetActor() != this) {
					FVector targetLocation = (*it).Actor->GetActorLocation();
					FString targetName = "Find target: " + (*it).Actor->GetName();
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, targetName);
					DrawDebugSphere(GetWorld(), targetLocation, 500.f, 32, FColor::Green, false, 10.f);
					if ((*it).Actor->GetRootComponent()->ComponentHasTag(FName("EnemyAircraft"))) {
						GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Target Accquired!"));
						FString message = "Target Accquired: " + (*it).Actor->GetName();
						GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, message);
						AActor* target = (*it).GetActor();
						CurrentTarget = Cast<APawn>(target);
					}
				}
			}
			if (CurrentTarget) {
				targetSelected = Cast<AEnemyPawn>(CurrentTarget);
				targetSelected->SetLockOnStatus(1);
			}
		}
		/** replication over */

}

void AFighterPawn::FireMissile() {
	// define fire missile function
	
	if (MissileRemain > 0) {
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
		// old guidance system
		// AActor* target = Cast<AActor>(CurrentTarget);
		missile->SetDamage(MissileDamage);
		missile->EngageTarget(CurrentTarget, this);
		MissileRemain--;
	}
}

void AFighterPawn::FireGuns() {
	// define fire guns function
	if (canFireCannon) {
		isFiringCannon = true;
		ToggleCannonSound();
	}
}

void AFighterPawn::ShootGuns()
{
	if (isFiringCannon && canFireCannon) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		FRotator SpawnRotation = GetActorRotation();
		FVector SpawnLocation = PlaneMesh->GetSocketLocation("Cannon");
		AGunShell* gunShell = GetWorld()->SpawnActor<AGunShell>(SpawnLocation, SpawnRotation, SpawnParams);
		canFireCannon = false;
		GetWorld()->GetTimerManager().SetTimer(GunCoolHandle, this, &AFighterPawn::ResetGunCool, 0.2f);
		canFireCannon = false;
	}

}

void AFighterPawn::StopFireGuns()
{
	isFiringCannon = false;
	ToggleCannonSound();
}

void AFighterPawn::ToggleCannonSound()
{
	if (isFiringCannon) {
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Start gun firing sound"));
		CannonSoundComponent->Play();
	}
	else {
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Stop gun firing sound"));
		CannonSoundComponent->Stop();
	}
}

void AFighterPawn::ClearSelectTarget()
{
	if (targetSelected) {
		targetSelected->SetLockOnStatus(0);
		LockOnGauge = 0.f;
		targetSelected = nullptr;
	}
}

void AFighterPawn::LockOnTarget()
{
	float Angle = 0.f;
	if (targetSelected) {
		FVector targetLocation = targetSelected->GetActorLocation();
		FVector targetVector = targetLocation - GetActorLocation();
		Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(PlaneMesh->GetForwardVector(), targetVector)));
		if (Angle <= 70) {
			LockOnGauge += (45.f*GetWorld()->GetDeltaSeconds());
		}
		else {
			LockOnGauge -= (60.f*GetWorld()->GetDeltaSeconds());
		}
		LockOnGauge = FMath::Clamp(LockOnGauge, 0.0f, 150.f);

		if (LockOnGauge >= 100 && Angle <= 70) {
			// play locked sound
			LockedSoundComponent->Play();
			LockingSoundComponent->Stop();
			// lock on target
			CurrentTarget = targetSelected;
			targetSelected->SetLockOnStatus(2);
		}
		else if (Angle <= 70) {
			// play locking sound
			CurrentTarget = nullptr;
			LockedSoundComponent->Stop();
			LockingSoundComponent->Play();
			targetSelected->SetLockOnStatus(1);
		}
		else {
			// target selected and chasing
			CurrentTarget = nullptr;
			LockedSoundComponent->Stop();
			LockingSoundComponent->Stop();
			targetSelected->SetLockOnStatus(1);
		}
	}
	else {
		LockedSoundComponent->Stop();
		LockingSoundComponent->Stop();
	}
	//FString message = "Lock on gauge: " + FString::SanitizeFloat(LockOnGauge);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, message);
}

void AFighterPawn::ResetGunCool()
{
	canFireCannon = true;
}

bool AFighterPawn::DetectMissile()
{
	bool result = false;
	// get all missile instance and decide if the missile is targeting the player
	TArray<AActor*> FoundMissiles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMissileCustom::StaticClass(), FoundMissiles);
	for (auto it = FoundMissiles.CreateIterator(); it; it++) {
		AActor* target = *it;
		AMissileCustom* incomingMissile = Cast<AMissileCustom>(target);
		if (incomingMissile->MissileOwner != this && incomingMissile->hasTarget()) {
			result = true;
			FString message = "missile incoming!";
			return result;
		}	
	}
	return result;
}

void AFighterPawn::ToggleMissileWarning()
{
	if (DetectMissile()) {
		// Play alarm sound and message for the player
		MissileWarningSoundComponent->Play();
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("start warning sound"));
	}
	else {
		// stop playing sound and message for the player
		MissileWarningSoundComponent->Stop();
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("stop warning sound"));
	}
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

int AFighterPawn::GetMissile() const
{
	return FMath::FloorToInt(MissileRemain);
}

int AFighterPawn::GetScore() const
{
	return FMath::FloorToInt(Score);
}

float AFighterPawn::GetPitch() const
{
	FRotator currentRotation = GetActorRotation();
	// get the Pitch
	float result = currentRotation.Pitch;
	return result;
}

float AFighterPawn::GetRoll() const
{
	FRotator currentRotation = GetActorRotation();
	// get the Roll
	float result = currentRotation.Roll;
	return result;
}

void AFighterPawn::SetScore(float val)
{
	Score += val;
	//
	AGameStateBase* CurrentGameState = GetWorld()->GetGameState();
	if (CurrentGameState) {
		AMyGameStateBase* myGameState = Cast<AMyGameStateBase>(CurrentGameState);
		myGameState->AddPlayerKills();
		myGameState->AddPlayerScore(FMath::FloorToInt(val));
	}

}

void AFighterPawn::AddKill()
{
	PlayerKills++;
}

int AFighterPawn::GetKill()
{
	return PlayerKills;
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

APawn* AFighterPawn::GetCurrentTarget()
{
	return CurrentTarget;
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
	// to be implemented
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

void AFighterPawn::SelfDestruction()
{
	SpawnExplosion();
	EngineSoundComponent->SetVolumeMultiplier(0.0f);
	//EngineSoundComponent->SetPaused(true);
	//EngineSoundComponent->PlaybackCompleted(EngineSoundComponent->GetAudioComponentID(),false);
	EngineSoundComponent->FadeOut(.5f, .0f);
	LockedSoundComponent->Stop();
	LockingSoundComponent->Stop();
	MissileWarningSoundComponent->Stop();
	MissileWarningSoundComponent->Deactivate();
	EngineSoundComponent->Stop();
	EngineSoundComponent->Deactivate();
	//EngineSoundComponent->DestroyComponent();
	//PlaneMesh->DestroyComponent();
	Destroy();
}

bool AFighterPawn::IsAlive()
{
	bool result = aircraftHP>0 ? true : false;
	return result;
}
