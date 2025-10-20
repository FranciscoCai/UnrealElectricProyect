// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ElectricPanel.generated.h"

class AElectricPanelStation;
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
	AElectricPanel();

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel")
	AElectricPanelStation* ElectricPanelStationOn; // 使用前向声明指针

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Electric")
	TArray<AElectricSpline*> Splines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Electric")
	TSubclassOf<AElectricCharacter> ElectricCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pose")
	bool poseOnStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction")
	bool bSpawnUp = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction")
	bool bSpawnDown = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction")
	bool bSpawnRight = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Direction")
	bool bSpawnLeft = false;

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
