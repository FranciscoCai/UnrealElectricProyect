// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElectricSpline.generated.h"

class USplineComponent;
class AElectricPanelStation;
class AElectricPanel;

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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Componente spline visible y editable en el editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Electric")
	USplineComponent* SplineComponent;

	// Handler que se une al evento (debe ser UFUNCTION para AddDynamic)
	UFUNCTION()
	void OpenClose(bool bIsOpen);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PanelStation")
	AElectricPanelStation* PanelStationStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PanelStation")
	AElectricPanelStation* PanelStationEnd;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PanelStation")
	bool PanelStartBool = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PanelStation")
	bool PanelEndBool = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PanelStation")
	bool SplineActivatableType = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PanelStation")
	bool IsActive = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel")
	TArray<AElectricPanel*> ActiveSplinePanels;
private:
	// Referencia cacheada al subsystem para desuscribir en EndPlay
	class UElectricSplineOpenCloseSubsystem* CachedSubsystem = nullptr;
};
