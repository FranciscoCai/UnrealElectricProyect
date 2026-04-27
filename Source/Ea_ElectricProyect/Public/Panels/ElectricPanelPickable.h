// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElectricPanel.h"
#include "ElectricPanelPickable.generated.h"

class AElectricPanelStation;

/**
 * 
 */
UCLASS()
class EA_ELECTRICPROYECT_API AElectricPanelPickable : public AElectricPanel
{
	GENERATED_BODY()
    public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pick")
	bool CanPick = true;
	// Now passes the station that the panel was snapped to
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Pick")
	void BP_OnSnapPanelEvent(AElectricPanelStation* Station);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Pick")
	void BP_OnPickPanelEvent();
};
