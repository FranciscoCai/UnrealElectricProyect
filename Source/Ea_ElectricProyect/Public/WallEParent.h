// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WallEParent.generated.h"

class UInputMappingContext;
class UInputAction;
class AElectricPanel_RobotStation;
class AElectricPanel;
class UPrimitiveComponent;
struct FInputActionValue;
struct FTimerHandle;

UCLASS()
class EA_ELECTRICPROYECT_API AWallEParent : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWallEParent();

	// Variable pública de tipo AElectricPanel_RobotStation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotStation")
	AElectricPanel_RobotStation* RobotStationRef;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Apply mapping when possessed by a player controller
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	/** Mapping context para este personaje (configurable en el editor) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* WallEMappingContext;

	/** Acción de movimiento (Vector2D) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractStation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* PickUp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* PickDown;

	/** Called por Enhanced Input cuando se recibe Move (Vector2D) */
	void Move(const FInputActionValue& Value);

	/** Ejecuta el movimiento en el Character (Right, Forward) */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	// --- Hold-to-interact para la estación (equivalente a ElectricPanel) ---
	// Called when the hold completes (children/blueprints can call/override if needed)
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void OnInteractStation();

	// Input callbacks: start / cancel interact-hold
	UFUNCTION(BlueprintCallable, Category = "Input")
	void BeginInteractStation();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void CancelInteractStation();

	// Duration required to hold to trigger OnInteractStation (seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta=(ClampMin="0.0"))
	float InteractStationHoldDuration = 1.5f;

	// Visible runtime state (for widgets/debug)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Input")
	bool bIsHoldingInteractStation = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Input")
	float CurrentInteractStationHoldTime = 0.0f;

private:
	// Prevent multiple triggers per hold
	bool bHasTriggeredInteractStation = false;

	// Timer handle for the hold timer
	FTimerHandle InteractStationTimerHandle;

public:
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnPickUpStarted();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnPickDownStarted();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotStation")
	AMainCamera* CameraToChange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotStation")
	float TransitionDuration;

private:
	// Reference to the currently held electric panel (if any)
	AElectricPanel* HeldPanel = nullptr;

	// Cached pointer to the PickBox component (found by name or tag at BeginPlay)
	UPrimitiveComponent* PickBoxComp = nullptr;
};
