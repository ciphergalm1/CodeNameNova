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
