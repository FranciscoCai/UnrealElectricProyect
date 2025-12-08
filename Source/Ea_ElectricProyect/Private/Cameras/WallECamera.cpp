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

// Define los l赤mites de pitch (puedes cambiarlos)
const float MinPitch = -45.0f;
const float MaxPitch = 0.0f;

void AWallECamera::AddOrbitInput(FVector2D Input)
{
	OrbitYaw += Input.X * 2.0f; // Orbita horizontal (mueve la c芍mara alrededor del objetivo)
	OrbitPitch = FMath::Clamp(OrbitPitch - Input.Y * 0.5f, MinPitch, MaxPitch); // Solo afecta la rotaci車n local
}

void AWallECamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(TargetActor)) return;
	if (bIsTransitioning) return;
	FVector TargetLocation = TargetActor->GetActorLocation();

	// Orbita horizontalmente (eje Yaw)
	float YawRad = FMath::DegreesToRadians(OrbitYaw);

	FVector Offset;
	Offset.X = OrbitDistance * FMath::Cos(YawRad);
	Offset.Y = OrbitDistance * FMath::Sin(YawRad);
	Offset.Z = 0.0f; // No se usa para la posici車n, solo para la rotaci車n

	// La posici車n de la c芍mara es alrededor del objetivo, pero con altura fija
	FVector DesiredCameraLocation = TargetLocation - Offset;
	DesiredCameraLocation.Z = TargetLocation.Z + CameraHeightOffset;

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
		FinalCameraLocation = HitResult.Location + HitResult.Normal * 10.0f;
	}

	FollowCamera->SetWorldLocation(FinalCameraLocation);

	// Solo rota la c芍mara sobre su propio eje Y (pitch local)
	FRotator CameraRot = (TargetLocation - FinalCameraLocation).Rotation();
	CameraRot.Pitch = OrbitPitch; // Aplica el pitch solo a la rotaci車n
	FollowCamera->SetWorldRotation(CameraRot);

	// Opcional: dibuja el trazo para depuraci車n
	// DrawDebugLine(GetWorld(), TargetLocation, FinalCameraLocation, FColor::Red, false, 0.1f, 0, 2.0f);
}



