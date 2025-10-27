// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricPanel_RobotStation.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"

void AElectricPanel_RobotStation::BeginPlay()
{
	Super::BeginPlay();
	// Additional initialization for RobotStation (if needed)
}

void AElectricPanel_RobotStation::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// Additional logic when this pawn is possessed (if needed)
}

void AElectricPanel_RobotStation::UnPossessed()
{
	Super::UnPossessed();
	// Additional cleanup when this pawn is unpossessed (if needed)
}

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

