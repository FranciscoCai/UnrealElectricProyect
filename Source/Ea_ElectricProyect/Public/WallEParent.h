// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WallEParent.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class EA_ELECTRICPROYECT_API AWallEParent : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWallEParent();

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

	/** Called por Enhanced Input cuando se recibe Move (Vector2D) */
	void Move(const FInputActionValue& Value);

	/** Ejecuta el movimiento en el Character (Right, Forward) */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);
};
