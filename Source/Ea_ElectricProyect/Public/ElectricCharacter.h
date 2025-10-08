// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ElectricCharacter.generated.h"

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

private:
	UPROPERTY()
	USplineComponent* SplineToFollow = nullptr;

	float CurrentDistanceOnSpline = 0.f;
	bool bMovingOnSpline = false;
};
