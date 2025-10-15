// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElectricPanel.h"
#include "ElectricPanelStation.generated.h"

UCLASS()
class EA_ELECTRICPROYECT_API AElectricPanelStation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElectricPanelStation();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel")
	AElectricPanel* ElectricPanelClass;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
