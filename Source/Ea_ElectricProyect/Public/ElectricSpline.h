// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElectricSpline.generated.h"

class USplineComponent;

UCLASS()
class EA_ELECTRICPROYECT_API AElectricSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElectricSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Componente spline visible y editable en el editor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Electric")
	USplineComponent* SplineComponent;
};
