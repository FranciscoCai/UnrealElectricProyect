// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
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
	AElectricPanelStation* ElectricPanelStationOn; // Ê¹ÓÃÇ°ÏòÉùÃ÷Ö¸Õë


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Electric")
	TSubclassOf<AElectricCharacter> ElectricCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pose")
	bool poseOnStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputMappingContext* PanelMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveUpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveDownAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveRightAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveLeftAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Interact;

	void SpawnAndPossessCharacter(int32 ControlIndex, bool bSpawnAtStart);

	UEnhancedInputLocalPlayerSubsystem* Subsystem;

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputUp();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputDown();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputRight();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputLeft();

protected:
	// Make overridable by C++ subclasses: this is called WHEN se completa el hold.
	// Los hijos deben sobreescribir esta función para ejecutar su comportamiento.
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void OnInteract();

	// --- Hold-to-interact implementation (base class) ---
public:
	// Tiempo requerido (segundos) para mantener Interact antes de ejecutar OnInteract.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta=(ClampMin="0.0"))
	float InteractHoldDuration = 1.5f;

	// Inicia la acción de mantener pulsado (llamado por el binding de input)
	UFUNCTION(BlueprintCallable, Category = "Input")
	void BeginInteract();

	// Cancela la acción de mantener pulsado (release / cancel)
	UFUNCTION(BlueprintCallable, Category = "Input")
	void CancelInteract();

protected:
	// Estado de hold (no editable por diseñador en runtime salvo Visualización)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Input")
	bool bIsHoldingInteract = false;

	// Tiempo acumulado desde que se empezó a mantener (si quieres mostrar progreso desde Blueprint/Widget debes actualizar esto desde un timer/Widget tick)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Input")
	float CurrentInteractHoldTime = 0.0f;

	// Evita ejecutar OnInteract varias veces por un mismo hold
	bool bHasTriggeredInteract = false;

	// Timer handle para no depender de Tick
	FTimerHandle InteractTimerHandle;

	// Callback cuando termina el timer
	void OnInteractTimerFinished();
};
