// Fill out your copyright notice in the Description page of Project Settings.

#include "WallECamera.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
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

	if (CachedPlayerController == nullptr)
	{
		CachedPlayerController = GetWorld()->GetFirstPlayerController();
	}

	if (!IsValid(TargetActor)) return;
	if (!bEnableFollowArea) return;
	if (bIsTransitioning) return;

	const FVector CamLocation = FollowCamera->GetComponentLocation(); // Usar la c¨¢mara f¨ªsica
	const FVector TargetLocation = TargetActor->GetActorLocation();

	const float Distance2D = FVector::Dist2D(CamLocation, TargetLocation);

	if (Distance2D > FollowAreaRadius && FollowAreaRadius > 0.0f)
	{
		const FVector2D CamXY(CamLocation.X, CamLocation.Y);
		const FVector2D TargetXY(TargetLocation.X, TargetLocation.Y);

		FVector2D Dir2D = TargetXY - CamXY;
		if (!Dir2D.IsNearlyZero())
		{
			Dir2D.Normalize();
			const FVector2D DesiredXY = TargetXY - Dir2D * FollowAreaRadius;
			const FVector DesiredLocation(DesiredXY.X, DesiredXY.Y, CamLocation.Z);

			const FVector NewLocation = FMath::VInterpTo(CamLocation, DesiredLocation, DeltaTime, MoveInterpSpeed);
			FollowCamera->SetWorldLocation(NewLocation); // Solo mueve la c¨¢mara
		}
	}

	// Rotaci¨®n de la c¨¢mara para mirar al target
	if (bLookAtTarget && TargetActor)
	{
		const FVector CurrentCamLocation = FollowCamera->GetComponentLocation();
		const FVector ToTarget = TargetLocation - CurrentCamLocation;
		if (!ToTarget.IsNearlyZero())
		{
			FRotator DesiredRot = ToTarget.Rotation();
			DesiredRot.Roll = 0.0f;
			const FRotator NewRot = FMath::RInterpTo(FollowCamera->GetComponentRotation(), DesiredRot, DeltaTime, LookInterpSpeed);
			FollowCamera->SetWorldRotation(NewRot); // Solo rota la c¨¢mara
		}
	}
}

