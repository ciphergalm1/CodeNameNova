// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeNameNuggets.h"
#include "MyPlayerState.h"

void AMyPlayerState::SetScore(int val)
{
	PlayerScore = val;
}

int AMyPlayerState::GetScore()
{
	return PlayerScore;
}

void AMyPlayerState::SetPlayerKills(int val)
{
	PlayerKills = val;
}

int AMyPlayerState::GetPlayerKills()
{
	return PlayerKills;
}
