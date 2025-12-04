// Fill out your copyright notice in the Description page of Project Settings.

#include "Cameras/WallECamera.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "CollisionQueryParams.h"

AWallECamera::AWallECamera()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWallECamera::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(TargetActor) && FollowCamera)
	{
		// Guarda la diferencia de altura inicial entre c芍mara y robot
		InitialZOffset = FollowCamera->GetComponentLocation().Z - TargetActor->GetActorLocation().Z;
	}
}

void AWallECamera::AddOrbitInput(FVector2D Input)
{
	OrbitYaw += Input.X * 2.0f;   // Sensibilidad ajustable
	OrbitPitch = FMath::Clamp(OrbitPitch - Input.Y * 2.0f, -80.0f, 80.0f);
}

void AWallECamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(TargetActor)) return;
	if (bIsTransitioning) return;
	FVector TargetLocation = TargetActor->GetActorLocation();
	float YawRad = FMath::DegreesToRadians(OrbitYaw);
	float PitchRad = FMath::DegreesToRadians(OrbitPitch);

	FVector Offset;
	Offset.X = OrbitDistance * FMath::Cos(PitchRad) * FMath::Cos(YawRad);
	Offset.Y = OrbitDistance * FMath::Cos(PitchRad) * FMath::Sin(YawRad);
	Offset.Z = OrbitDistance * FMath::Sin(PitchRad);

	FVector DesiredCameraLocation = TargetLocation - Offset;
	DesiredCameraLocation.Z = TargetLocation.Z + InitialZOffset;

	// --- Comprobaci車n de colisi車n para evitar atravesar paredes ---
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (TargetActor)
	{
		Params.AddIgnoredActor(TargetActor);
	}

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TargetLocation,
		DesiredCameraLocation,
		ECC_Visibility,
		Params
	);

	FVector FinalCameraLocation = DesiredCameraLocation;
	if (bHit)
	{
		// Coloca la c芍mara justo antes de la colisi車n
		FinalCameraLocation = HitResult.Location + HitResult.Normal * 10.0f; // 10 unidades de separaci車n
	}

	FollowCamera->SetWorldLocation(FinalCameraLocation);

	FRotator CameraRot = (TargetLocation - FinalCameraLocation).Rotation();
	FollowCamera->SetWorldRotation(CameraRot);

	// Opcional: dibuja el trazo para depuraci車n
	// DrawDebugLine(GetWorld(), TargetLocation, FinalCameraLocation, FColor::Red, false, 0.1f, 0, 2.0f);
}


