// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ElectricPanel.generated.h"

class AElectricSpline;
class AElectricCharacter;
class UInputAction;
class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;
struct FInputActionValue;

UCLASS()
class EA_ELECTRICPROYECT_API AElectricPanel : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AElectricPanel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when this pawn becomes possessed by a controller (override to apply mapping context each possession)
	virtual void PossessedBy(AController* NewController) override;

	// Quitar el mapping context al dejar de ser pose¨ªdo
	virtual void UnPossessed() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Referencias a los 4 splines, editables en Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Electric")
	TArray<AElectricSpline*> Splines;

	// Clase del personaje a spawnear, editable en Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Electric")
	TSubclassOf<AElectricCharacter> ElectricCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pose")
	bool poseOnStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction")
	bool bSpawnUp = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction")
	bool bSpawnDown = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction")
	bool bSpawnRight = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction")
	bool bSpawnLeft = false;

	// Input mappings, editables en Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputMappingContext* PanelMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveUpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveDownAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveLeftAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveRightAction;


private:
	void SpawnAndPossessCharacter(int32 SplineIndex, bool bSpawnAtStart);
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem;

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputUp();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputDown();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputLeft();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputRight();
};


