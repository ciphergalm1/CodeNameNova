// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CODENAMENUGGETS_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	int PlayerScore = 0;
	
	UFUNCTION(Category = PlayerState, BlueprintCallable)
	void SetScore(int val);

	UFUNCTION(Category = PlayerState, BlueprintPure)
	int GetScore();

	int PlayerKills = 0;

	UFUNCTION(Category = PlayerState, BlueprintCallable)
	void SetPlayerKills(int val);

	UFUNCTION(Category = PlayerState, BlueprintPure)
	int GetPlayerKills();
	
};
