// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricPanelStation.h"
#include "ElectricPanel.h"
#include "ElectricSpline.h"


// Sets default values
AElectricPanelStation::AElectricPanelStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
        case 2: bSpawn = bSpawnDown; break;
        case 3: bSpawn = bSpawnRight;  break;
        default: break;
        }

        if (bSpawn)
        {
            Spline->PanelStationStart = this;
        }
        else
        {
            Spline->PanelStationEnd = this;
        }
    }
}

