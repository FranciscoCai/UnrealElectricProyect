// Fill out your copyright notice in the Description page of Project Settings.

#include "Panels/ElectricPanel_PowerStrip.h"
#include "Ea_ElectricProyectPlayerController.h"
#include "GameFramework/PlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"

void AElectricPanel_PowerStrip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	AEa_ElectricProyectPlayerController* EaPC = Cast<AEa_ElectricProyectPlayerController>(GetController());
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (EaPC)
		{
			// Start directional hold on Started, cancel on Canceled
			EnhancedInput->BindAction(EaPC->MoveUpAction, ETriggerEvent::Started, this, &AElectricPanel_PowerStrip::OnPowerStripInputUp);

			EnhancedInput->BindAction(EaPC->MoveLeftAction, ETriggerEvent::Started, this, &AElectricPanel_PowerStrip::OnPowerStripInputLeft);

			EnhancedInput->BindAction(EaPC->MoveDownAction, ETriggerEvent::Started, this, &AElectricPanel_PowerStrip::OnPowerStripInputDown);

			EnhancedInput->BindAction(EaPC->MoveRightAction, ETriggerEvent::Started, this, &AElectricPanel_PowerStrip::OnPowerStripInputRight);
		}
	}
}
void AElectricPanel_PowerStrip::OnPowerStripInputUp()
{
	if (PowerStripChange.IsValidIndex(0))
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->Possess(PowerStripChange[0]);
		}
	}
}
void AElectricPanel_PowerStrip::OnPowerStripInputLeft()
{
	if (PowerStripChange.IsValidIndex(1))
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->Possess(PowerStripChange[1]);
		}
	}
}
void AElectricPanel_PowerStrip::OnPowerStripInputDown()
{
	if (PowerStripChange.IsValidIndex(2))
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->Possess(PowerStripChange[2]);
		}
	}
}
void AElectricPanel_PowerStrip::OnPowerStripInputRight()
{
	if (PowerStripChange.IsValidIndex(3))
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->Possess(PowerStripChange[3]);
		}
	}
}
