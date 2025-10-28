// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCamera.h"
#include "GameFramework/Pawn.h"
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

	/** Target pawn to keep within the follow area (can be set in editor). If null, will try to use the player's pawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
	APawn* TargetActor = nullptr;

	/** Radius (world units) of the allowed area around the target. Camera will move if distance > this radius. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow", meta = (ClampMin = "0.0"))
	float FollowAreaRadius = 600.0f;

	/** Interp speed used when moving the camera toward the allowed area boundary. Higher = faster. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow", meta = (ClampMin = "0.0"))
	float MoveInterpSpeed = 6.0f;

	/** Minimum allowed proximity (2D). Camera will be clamped to this distance from the pawn (XY only). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow", meta = (ClampMin = "0.0"))
	float MinFollowRadius = 300.0f;

	/** Interp speed used when clamping camera to minimum radius. Higher = snappier clamp. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow", meta = (ClampMin = "0.0"))
	float MinClampInterpSpeed = 8.0f;

	/** Maximum XY speed (units/sec) allowed when the camera is being clamped to the minimum radius.
     Set to 0 to disable per-frame speed limiting (only interpolation will apply). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow", meta = (ClampMin = "0.0"))
	float MaxMinMoveSpeed = 600.0f;

	/** When false, the camera will not try to correct its position. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
	bool bEnableFollowArea = true;
};
