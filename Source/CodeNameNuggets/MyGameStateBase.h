// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameStateBase.h"
#include "MyGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class CODENAMENUGGETS_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:

	int EnemyInLevel = 0;

	UFUNCTION(Category = PlayerState, BlueprintCallable)
	void SetEnemyInLevel(int val);

	UFUNCTION(Category = PlayerState, BlueprintPure)
	int GetEnemyInLevel();

	int PlayerKills =0;

	UFUNCTION(Category = PlayerState, BlueprintCallable)
	void AddPlayerKills();

	UFUNCTION(Category = PlayerState, BlueprintPure)
	int GetPlayerKills();

	int PlayerScore = 0;

	UFUNCTION(Category = PlayerState, BlueprintCallable)
	void AddPlayerScore(int ScoreDelta);

	UFUNCTION(Category = PlayerState, BlueprintPure)
	int GetPlayerScore();
	
	
	
	
};
