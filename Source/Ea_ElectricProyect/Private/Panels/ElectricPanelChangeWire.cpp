// Fill out your copyright notice in the Description page of Project Settings.


#include "Panels/ElectricPanelChangeWire.h"


void AElectricPanelChangeWire::BroadcastWireInteract()
{
	OnWireInteract.Broadcast();
}

