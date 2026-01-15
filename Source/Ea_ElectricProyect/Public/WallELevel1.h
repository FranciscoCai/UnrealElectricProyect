// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WallEParent.h"
#include "WallELevel1.generated.h"

/**
 * 
 */
UCLASS()
class EA_ELECTRICPROYECT_API AWallELevel1 : public AWallEParent
{
	GENERATED_BODY()
	
public:
	virtual void OnPressE() override;
	virtual void OnPressQ() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State")
	bool bDown = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bTransition = false;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "RobotStation")
	void WallEUp();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "RobotStation")
	void WallEDown();
};
