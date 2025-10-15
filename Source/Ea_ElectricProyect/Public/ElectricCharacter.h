// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "ElectricCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;
class USplineComponent;

UCLASS()
class EA_ELECTRICPROYECT_API AElectricCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AElectricCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetSpline(USplineComponent* InSpline, float InitialDistance = 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Electric")
	float SplineMoveSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Electric")
	bool bReverseSplineDirection = false;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* ElectricMoveMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveUpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveDownAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float MoveDirection = 0.f;

private:
	UPROPERTY()
	USplineComponent* SplineToFollow = nullptr;

	float CurrentDistanceOnSpline = 0.f;
	bool bMovingOnSpline = false;

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputGoUp();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputBackUp();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputGoDown();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputBackDown();

};
