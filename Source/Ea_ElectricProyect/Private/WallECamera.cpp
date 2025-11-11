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
}

void AWallECamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Keep cached player controller reference (used only if needed elsewhere)
	if (CachedPlayerController == nullptr)
	{
		CachedPlayerController = GetWorld()->GetFirstPlayerController();
	}

	if (!IsValid(TargetActor)) return;

	if (!bEnableFollowArea) return;
	if (bIsTransitioning) return;

	const FVector CamLocation = GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();

	// Use 2D distance (ignore Z)
	const float Distance2D = FVector::Dist2D(CamLocation, TargetLocation);

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

	// Make the camera look at the target (optional, smooth)
	if (bLookAtTarget && TargetActor)
	{
		const FVector CurrentCamLocation = GetActorLocation(); // updated location
		const FVector ToTarget = TargetLocation - CurrentCamLocation;
		if (!ToTarget.IsNearlyZero())
		{
			FRotator DesiredRot = ToTarget.Rotation();
			DesiredRot.Roll = 0.0f; // avoid roll
			const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), DesiredRot, DeltaTime, LookInterpSpeed);
			SetActorRotation(NewRot);
		}
	}
}

