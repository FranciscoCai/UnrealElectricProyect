// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WallEParent.generated.h"

class UInputMappingContext;
class UInputAction;
class AElectricPanel_RobotStation;
class AElectricPanel;
class AMainCamera;
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

	/** Acci¨®n de movimiento (Vector2D) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractStation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* PickUp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* PressE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* PressQ;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* WallELook;

	/** Called por Enhanced Input cuando se recibe Move (Vector2D) */
	void Move(const FInputActionValue& Value);

	/** Ejecuta el movimiento en el Character (Right, Forward) */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	// --- Hold-to-interact para la estaci¨®n (equivalente a ElectricPanel) ---
	// Called when the hold completes (children/blueprints can call/override if needed)
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void OnInteractStation();

	// Input callbacks: start / cancel interact-hold
	UFUNCTION(BlueprintCallable, Category = "Input")
	void BeginInteractStation();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void CancelInteractStation();
	// Duration required to hold to trigger OnInteractStation (seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (ClampMin = "0.0"))
	float InteractStationHoldDuration = 1.5f;

	// Visible runtime state (for widgets/debug)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Input")
	bool bIsHoldingInteractStation = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Input")
	float CurrentInteractStationHoldTime = 0.0f;

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void OnPressE();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void OnPressQ();


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
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnPickUpOrDownStarted();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotStation")
	AMainCamera* CameraToChange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotStation")
	float TransitionDuration;

private:

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel")
	AElectricPanel* HeldPanel = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MovementSpeed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UPrimitiveComponent* PickBoxComp = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnMoveReleased(const FInputActionValue& Value);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (ClampMin = "0.0"))
	float InteractHoldDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bFreeCam = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State")
	bool bPick = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bPickTransition = false;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector2D MovementDirection;

	// Pickup interpolation properties (moved to public so UPROPERTY is allowed)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pick")
	float PickInterpDuration = 0.25f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pick")
	float PickElapsedTime = 0.0f;

	/** Maximum look rotation speed (degrees per second) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "0.0"))
	float MaxLookSpeed = 180.0f;

protected:
	/** Called por Enhanced Input cuando se recibe WallELook (Vector2D) */
	UFUNCTION()
	void OnLookInput(const FInputActionValue& Value);

private:
	FRotator CurrentInterpRotation;

	// Transforms used for interpolation (runtime, no UPROPERTY needed)
	FTransform PickStartTransform;
	FTransform PickTargetTransform;

	// Timer handle for interpolation
	FTimerHandle PickInterpTimerHandle;

	// State flag used while interpolating the panel toward socket
	bool bIsPickingInterp = false;

	// Timer-driven interpolation callback (declared so SetTimer can bind to it)
	void TickPickInterp();
};