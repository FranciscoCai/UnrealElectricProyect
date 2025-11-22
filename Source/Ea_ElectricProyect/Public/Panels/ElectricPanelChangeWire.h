// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Panels/ElectricPanelPickable.h"
#include "ElectricPanelChangeWire.generated.h"

// Evento sin par¨¢metros
DECLARE_EVENT(AElectricPanelChangeWire, FOnWireInteract)

UCLASS()
class EA_ELECTRICPROYECT_API AElectricPanelChangeWire : public AElectricPanelPickable
{
	GENERATED_BODY()
public:
	FOnWireInteract OnWireInteract;

	void BroadcastWireInteract();

protected:
	virtual void OnInteract() override;
};
