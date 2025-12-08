// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCamera.h"
#include "WallECamera.generated.h"

class APawn;

/**
 * Camera that keeps itself inside a configurable area relative to the player's pawn.
 * If the player pawn goes outside the area, the camera approaches until it is back inside.
 */
UCLASS()
class EA_ELECTRICPROYECT_API AWallECamera : public AMainCamera
{
	GENERATED_BODY()

public:
	AWallECamera();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	/** Pawn objetivo a orbitar (debe asignarse en el editor o en runtime) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	APawn* TargetActor = nullptr;

	/** Ángulo yaw de la órbita */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	float OrbitYaw = 0.0f;

	/** Ángulo pitch de la órbita */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	float OrbitPitch = 10.0f;

	/** Distancia de la cámara al objetivo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	float OrbitDistance = 400.0f;

	/** Altura inicial de la cámara respecto al objetivo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	float CameraHeightOffset = 200.0f;

	/** Llama esto desde el input para rotar la cámara */
	void AddOrbitInput(FVector2D Input);

protected:
	// Diferencia de altura inicial entre cámara y robot
	float InitialZOffset = 0.0f;
};
