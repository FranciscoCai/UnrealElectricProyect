// Fill out your copyright notice in the Description page of Project Settings.

#include "Panels/ElectricPanelStation.h"
#include "Panels/ElectricPanel.h"
#include "ElectricSpline.h"

// Sets default values
AElectricPanelStation::AElectricPanelStation()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AElectricPanelStation::BeginPlay()
{
	Super::BeginPlay();
	if (ElectricPanelClass)
	{
		SetelectricPanelInformation(ElectricPanelClass);
	}
	UpdateSplinesPanelStation();
}

// Called every frame
void AElectricPanelStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AElectricPanelStation::SetelectricPanelInformation(AElectricPanel* PanelAttach)
{
	if (PanelAttach)
	{
		PanelAttach->ElectricPanelStationOn = this;
	}
}
void AElectricPanelStation::SetelectricStationPanelInformation(AElectricPanel* PanelAttach)
{
	if (PanelAttach)
	{
		ElectricPanelClass = PanelAttach;
	}
}

void AElectricPanelStation::UpdateSplinesPanelStation()
{
	for (int32 i = 0; i < Splines.Num(); ++i)
	{
		AElectricSpline* Spline = Splines[i];
		if (!Spline) continue;

		bool bSpawn = false;
		switch (i)
		{
		case 0: bSpawn = bSpawnUp;    break;
		case 1: bSpawn = bSpawnLeft;  break;
		case 2: bSpawn = bSpawnDown;  break;
		case 3: bSpawn = bSpawnRight; break;
		default: break;
		}

		// Safe resolution of the "limit" flag: only read ElectricPanelClass if it's valid.
		bool bLimit = false;
		if (ElectricPanelClass)
		{
			switch (i)
			{
			case 0: bLimit = ElectricPanelClass->LimitUp;    break;
			case 1: bLimit = ElectricPanelClass->LimitLeft;  break;
			case 2: bLimit = ElectricPanelClass->LimitDown;  break;
			case 3: bLimit = ElectricPanelClass->LimitRight; break;
			default: bLimit = false; break;
			}
		}
		else
		{
			// Fallback behavior when no panel is attached: keep false (or change this to another default if desired)
			bLimit = false;
		}

		// Update spline flags and pointers
		if (bSpawn)
		{
			Spline->PanelStationStart = this;
			Spline->PanelStartBool = bLimit;
		}
		else
		{
			Spline->PanelStationEnd = this;
			Spline->PanelEndBool = bLimit;
		}
	}
}

