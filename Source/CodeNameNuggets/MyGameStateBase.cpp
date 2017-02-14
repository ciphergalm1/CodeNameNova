// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "MyGameStateBase.h"

void AMyGameStateBase::SetEnemyInLevel(int val)
{
	EnemyInLevel = val;
}

int AMyGameStateBase::GetEnemyInLevel()
{
	return EnemyInLevel;
}

void AMyGameStateBase::AddPlayerKills()
{
	PlayerKills++;
}

int AMyGameStateBase::GetPlayerKills()
{
	return PlayerKills;
}

void AMyGameStateBase::AddPlayerScore(int ScoreDelta)
{
	PlayerScore += ScoreDelta;
}

int AMyGameStateBase::GetPlayerScore()
{
	return PlayerScore;
}
