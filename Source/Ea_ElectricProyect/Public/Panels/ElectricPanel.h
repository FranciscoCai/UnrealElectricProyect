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
	AElectricPanelStation* ElectricPanelStationOn; // 使用前向声明指针


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Electric")
	TSubclassOf<AElectricCharacter> ElectricCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pose")
	bool poseOnStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Interact;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Direction")
	bool LimitUp = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Direction")
	bool LimitLeft = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Direction")
	bool LimitDown = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Direction")
	bool LimitRight = true;

	void SpawnAndPossessCharacter(int32 ControlIndex, bool bSpawnAtStart, bool bReverseInput);

	UEnhancedInputLocalPlayerSubsystem* Subsystem;

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputUp();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputDown();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputRight();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputLeft();

	// Release handlers for directional inputs (cancel the directional hold)
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputUpReleased();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputDownReleased();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputRightReleased();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnInputLeftReleased();

	// Duration required to hold a directional input before spawning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta=(ClampMin="0.0"))
	float DirectionalHoldDuration = 0.3f;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Input")
	void StartDirectionBP(int startDirection);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Input")
	void CancelDirectionBP(int cancelDirection);



protected:
	// Timer & pending state for directional hold-to-spawn
	FTimerHandle DirectionalHoldTimerHandle;
	int32 PendingDirectionalControlIndex = -1;
	bool bPendingDirectionalSpawnAtStart = false;
	bool bPendingDirectionalReverseInput = false;
	bool bHasPendingDirectionalSpawn = false;

	// Tiempo requerido (segundos) para mantener Interact antes de ejecutar OnInteract.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta=(ClampMin="0.0"))
	float InteractHoldDuration = 1.5f;

	// Inicia la acci髇 de mantener pulsado (llamado por el binding de input)
	UFUNCTION(BlueprintCallable, Category = "Input")
	void BeginInteract();

	// Cancela la acci髇 de mantener pulsado (release / cancel)
	UFUNCTION(BlueprintCallable, Category = "Input")
	void CancelInteract();

protected:
	// Estado de hold (no editable por dise馻dor en runtime salvo Visualizaci髇)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Input")
	bool bIsHoldingInteract = false;

	// Tiempo acumulado desde que se empez?a mantener (si quieres mostrar progreso desde Blueprint/Widget debes actualizar esto desde un timer/Widget tick)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Input")
	float CurrentInteractHoldTime = 0.0f;

	// Evita ejecutar OnInteract varias veces por un mismo hold
	bool bHasTriggeredInteract = false;

	// Timer handle para no depender de Tick
	FTimerHandle InteractTimerHandle;

	// Callback cuando termina el timer
	void OnInteractTimerFinished();

	virtual void OnInteract();

	// Callback cuando termina el timer direccional
	void OnDirectionalHoldFinished();
	// Cancela hold direccional
	void CancelDirectionalHold();

protected:
	// Pending spawn data when a directional input started hold
	int32 PendingControlIndex = -1;
	bool bPendingSpawnAtStart = false;
	bool bPendingReverseInput = false;
	bool bHasPendingSpawn = false;

private:
	bool poseElectricCharacter = false;
};
