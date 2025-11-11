// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricPanel_RobotStation.h"
#include "Engine/Engine.h"
#include "WallEParent.h"
#include "GameFramework/PlayerController.h"


void AElectricPanel_RobotStation::OnInteract_Implementation()
{

	// Only pure C++ instances reach this point.
	if (WallEParentRef)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			ChangeCameraStart();
			PC->Possess(WallEParentRef);
		}
	}
}

