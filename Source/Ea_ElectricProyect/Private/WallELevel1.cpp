// Fill out your copyright notice in the Description page of Project Settings.


#include "WallELevel1.h"

void AWallELevel1::OnPressE()
{
	if (bTransition) return;
	UE_LOG(LogTemp, Warning, TEXT("Presionaste E en WallELevel1"));
	bTransition = true;
	bDown = false;
	WallEUp();
}

void AWallELevel1::OnPressQ()
{
	if (bTransition) return;
	UE_LOG(LogTemp, Warning, TEXT("Presionaste E en WallELevel1"));
	bTransition = true;
	bDown = true;
	WallEDown();
}

