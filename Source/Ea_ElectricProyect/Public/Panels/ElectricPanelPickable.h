// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElectricPanel.h"
#include "ElectricPanelPickable.generated.h"

/**
 * 
 */
UCLASS()
class EA_ELECTRICPROYECT_API AElectricPanelPickable : public AElectricPanel
{
	GENERATED_BODY()
    public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pick")
	bool CanPick = true;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Pick")
	void OnSnapPanelEvent();
};
