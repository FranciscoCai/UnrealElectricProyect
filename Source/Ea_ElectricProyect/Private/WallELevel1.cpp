// Fill out your copyright notice in the Description page of Project Settings.


#include "WallELevel1.h"

void AWallELevel1::OnPressE()
{
	if (bTransition||!bDown) return;
	bTransition = true;
	bDown = false;
	WallEUp();
}

void AWallELevel1::OnPressQ()
{
	if (bTransition||bDown) return;
	bTransition = true;
	bDown = true;
	WallEDown();
}

