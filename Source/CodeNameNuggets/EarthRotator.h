// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "EarthRotator.generated.h"

UCLASS()
class CODENAMENUGGETS_API AEarthRotator : public AActor
{
	GENERATED_BODY()

		/* StaticMesh component that will be the visuals for the earth */
		UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* EarthMesh;

public:	
	// Sets default values for this actor's properties
	AEarthRotator();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void Rotate(float DeltaSeconds);

private:
	float RotateSpeed;
	
};
