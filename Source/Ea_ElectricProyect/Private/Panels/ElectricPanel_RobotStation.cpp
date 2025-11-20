// Fill out your copyright notice in the Description page of Project Settings.

#include "Panels/ElectricPanel_RobotStation.h"
#include "Engine/Engine.h"
#include "WallEParent.h"
#include "GameFramework/PlayerController.h"


void AElectricPanel_RobotStation::OnInteract_Implementation()
{

	// Only pure C++ instances reach this point.
	if (WallEParentRef)
	{
		if (CameraToBack->bIsTransitioning)
		{
			return;
		}
		if (AEa_ElectricProyectPlayerController* PC = Cast<AEa_ElectricProyectPlayerController>(GetController()))
		{
			PC->ChangeCameraStart(CameraToGo, GoDuration);
			PC->Possess(WallEParentRef);
		}
	}
}

