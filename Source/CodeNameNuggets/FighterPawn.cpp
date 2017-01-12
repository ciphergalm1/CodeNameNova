// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "FighterPawn.h"

AFighterPawn::AFighterPawn()
{
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

	// Create static mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	RootComponent = PlaneMesh;

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

	// Setting aircraft parameters
	Acceleration = 400.f;
	TurnSpeed = 80.f;
	MaxSpeed = 4000.f;
	MinSpeed = 700.f;
	CurrentForwardSpeed = 500.f;
	CurrentYawSpeed = 0.0f;
	aircraftHP = 100.f;
	SpeedDelta = MaxSpeed - MinSpeed;
}

void AFighterPawn::Tick(float DeltaSeconds)
{
	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);

	// Move plane forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0, 0, 0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation);

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
}

void AFighterPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("You hit something!"));
	// Deflect along the surface when we collide.
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
}

void AFighterPawn::ThrustInput(float Val)
{
	// Is there no input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	// If input is not held down, reduce speed
	float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);

	CurrentThrustRatio = (CurrentForwardSpeed - MinSpeed )/ SpeedDelta;
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

}

void AFighterPawn::FireGuns() {
	// define fire guns function

}
