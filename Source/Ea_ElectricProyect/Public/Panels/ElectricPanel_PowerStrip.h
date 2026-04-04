// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Panels/ElectricPanel.h"
#include "ElectricPanel_PowerStrip.generated.h"

/**
 * 
 */
UCLASS()
class EA_ELECTRICPROYECT_API AElectricPanel_PowerStrip : public AElectricPanel
{
	GENERATED_BODY()
protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnPowerStripInputUp();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnPowerStripInputLeft();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnPowerStripInputDown();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnPowerStripInputRight();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TArray<AElectricPanel_PowerStrip*> PowerStripChange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool hasPossessed = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool canReceiveInput = true;
};
