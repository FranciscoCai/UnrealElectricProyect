// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElectricSpline.generated.h"

class USplineComponent;
class AElectricPanelStation;

UCLASS()
class EA_ELECTRICPROYECT_API AElectricSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElectricSpline();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PanelStation")
	AElectricPanelStation* PanelStationStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PanelStation")
	AElectricPanelStation* PanelStationEnd;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Componente spline visible y editable en el editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Electric")
	USplineComponent* SplineComponent;
};
