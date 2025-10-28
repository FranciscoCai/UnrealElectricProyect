// Fill out your copyright notice in the Description page of Project Settings.

#include "WallECamera.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Math/UnrealMathUtility.h"

AWallECamera::AWallECamera()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWallECamera::BeginPlay()
{
	Super::BeginPlay();

	// If TargetActor not set in editor, try to use the player's pawn
	if (!TargetActor)
	{
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				TargetActor = Pawn;
			}
		}
	}
}

void AWallECamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Ensure TargetActor follows whatever pawn the player controller currently possesses.
	// This covers runtime pawn switches.
	if (CachedPlayerController == nullptr)
	{
		CachedPlayerController = GetWorld()->GetFirstPlayerController();
	}

	if (CachedPlayerController)
	{
		APawn* CurrentPawn = CachedPlayerController->GetPawn();
		if (CurrentPawn && CurrentPawn != TargetActor)
		{
			TargetActor = CurrentPawn;
		}
	}

	if (!bEnableFollowArea) return;
	if (!TargetActor) return;

	const FVector CamLocation = GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();

	// Use 2D distance (ignore Z)
	const float Distance2D = FVector::Dist2D(CamLocation, TargetLocation);

	// --- Minimum-proximity clamp for the CAMERA (prevent camera from entering pawn-centered circle) ---
	if (MinFollowRadius > 0.0f && Distance2D < MinFollowRadius)
	{
		// Work in XY plane
		const FVector2D CamXY(CamLocation.X, CamLocation.Y);
		const FVector2D TargetXY(TargetLocation.X, TargetLocation.Y);

		// Direction from pawn to camera
		FVector2D Dir = CamXY - TargetXY;
		if (Dir.IsNearlyZero())
		{
			// avoid zero-length (if camera exactly over pawn), push to +X
			Dir = FVector2D(1.0f, 0.0f);
		}
		Dir.Normalize();

		// Desired camera XY on the boundary of MinFollowRadius
		const FVector2D DesiredXY = TargetXY + Dir * MinFollowRadius;
		const FVector DesiredLocation(DesiredXY.X, DesiredXY.Y, CamLocation.Z);

		// Smooth toward desired location using interpolation
		FVector InterpLocation = FMath::VInterpTo(CamLocation, DesiredLocation, DeltaTime, MinClampInterpSpeed);

		// Optionally clamp max XY displacement this frame to avoid big jumps
		if (MaxMinMoveSpeed > 0.0f)
		{
			const FVector2D DeltaXY(InterpLocation.X - CamLocation.X, InterpLocation.Y - CamLocation.Y);
			const float DeltaLen = DeltaXY.Size();
			const float MaxStep = MaxMinMoveSpeed * DeltaTime;

			if (DeltaLen > MaxStep && DeltaLen > KINDA_SMALL_NUMBER)
			{
				const FVector2D ClampedDeltaXY = DeltaXY.GetSafeNormal() * MaxStep;
				InterpLocation.X = CamLocation.X + ClampedDeltaXY.X;
				InterpLocation.Y = CamLocation.Y + ClampedDeltaXY.Y;
			}
		}

		// Apply clamped/smoothed location
		SetActorLocation(InterpLocation);

		// Skip other camera logic this frame
		return;
	}

	// If outside the allowed radius (in XY), move camera toward target until it lies on the radius boundary (inside the area).
	if (Distance2D > FollowAreaRadius && FollowAreaRadius > 0.0f)
	{
		// Work in XY plane
		const FVector2D CamXY(CamLocation.X, CamLocation.Y);
		const FVector2D TargetXY(TargetLocation.X, TargetLocation.Y);

		FVector2D Dir2D = TargetXY - CamXY;
		if (!Dir2D.IsNearlyZero())
		{
			Dir2D.Normalize();

			// Desired XY is at FollowAreaRadius from target along line from target to camera
			const FVector2D DesiredXY = TargetXY - Dir2D * FollowAreaRadius;

			// Preserve camera Z
			const FVector DesiredLocation(DesiredXY.X, DesiredXY.Y, CamLocation.Z);

			// Smoothly interpolate to desired location (only XY changes; Z stays as CamLocation.Z)
			const FVector NewLocation = FMath::VInterpTo(CamLocation, DesiredLocation, DeltaTime, MoveInterpSpeed);

			SetActorLocation(NewLocation);
		}
	}
	// else: inside allowed area -> do nothing (camera stays still)
}

