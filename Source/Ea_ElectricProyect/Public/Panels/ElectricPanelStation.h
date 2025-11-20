// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElectricPanelStation.generated.h"


class AElectricSpline;
class AElectricPanel;
class ACameraLookAtSpline;

UCLASS()
class EA_ELECTRICPROYECT_API AElectricPanelStation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElectricPanelStation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel")
	AElectricPanel* ElectricPanelClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction")
	bool bSpawnUp = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction")
	bool bSpawnLeft = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction")
	bool bSpawnDown = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction")
	bool bSpawnRight = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electric")
	TArray<AElectricSpline*> Splines;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electric")
	TArray<ACameraLookAtSpline*> LookAtSplineChange;

	UFUNCTION(BlueprintCallable, Category = "Electric")
	void SetelectricPanelInformation(AElectricPanel* PanelAttach);
	UFUNCTION(BlueprintCallable, Category = "Electric")
	void SetelectricStationPanelInformation(AElectricPanel* PanelAttach);

	UFUNCTION(BlueprintCallable, Category = "Electric")
	void UpdateSplinesPanelStation();
};
