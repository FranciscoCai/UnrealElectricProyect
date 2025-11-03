// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricPanel_RobotStation.h"
#include "Engine/Engine.h"
#include "WallEParent.h"
#include "GameFramework/PlayerController.h"


void AElectricPanel_RobotStation::OnInteract()
{
	// If a WallEParent reference is assigned, transfer local player possession to it.
	if (WallEParentRef)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->Possess(WallEParentRef);

		}
	}
}

