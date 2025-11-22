// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElectricPanel.h"
#include "ElectricPanel_RobotStation.generated.h"
class AWallEParent;

/**
 * 
 */
UCLASS()
class EA_ELECTRICPROYECT_API AElectricPanel_RobotStation : public AElectricPanel
{
	GENERATED_BODY()

protected:
	// Override the base class interaction handler (C++ override)
	virtual void OnInteract() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotStation")
	AWallEParent* WallEParentRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotStation")
	AMainCamera* CameraToGo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotStation")
	float GoDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotStation")
	AMainCamera* CameraToBack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotStation")
	float BackDuration;
};
