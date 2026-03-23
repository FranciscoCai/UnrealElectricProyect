// Fill out your copyright notice in the Description page of Project Settings.

#include "Cameras/WallECamera.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "CollisionQueryParams.h"


namespace
{
	constexpr float MinPitch = -45.0f;
	constexpr float MaxPitch = 5.0f;
}

AWallECamera::AWallECamera()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWallECamera::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(TargetActor) && FollowCamera)
	{
		// Guarda la diferencia de altura inicial entre cámara y robot
		InitialZOffset = FollowCamera->GetComponentLocation().Z - TargetActor->GetActorLocation().Z;
	}

	// Si se pidió aplicar la órbita inicial en BeginPlay, hazlo ahora
	if (bApplyInitialOrbitAtBeginPlay)
	{
		SetOrbitRotation(InitialOrbitYaw, InitialOrbitPitch, true);
	}
}


void AWallECamera::SetOrbitRotation(float NewYaw, float NewPitch, bool bApplyPosition /*= true*/)
{
	// Update orbit values, clamping pitch to allowed range
	OrbitYaw = NewYaw;
	OrbitPitch = FMath::Clamp(NewPitch, MinPitch, MaxPitch);

	if (!FollowCamera) return;

	// If requested, compute and apply camera position/rotation consistent with the orbit
	if (bApplyPosition && IsValid(TargetActor))
	{
		FVector TargetLocation = TargetActor->GetActorLocation();

		// Orbita horizontalmente (eje Yaw)
		const float YawRad = FMath::DegreesToRadians(OrbitYaw);

		FVector Offset;
		Offset.X = OrbitDistance * FMath::Cos(YawRad);
		Offset.Y = OrbitDistance * FMath::Sin(YawRad);
		Offset.Z = 0.0f;

		FVector DesiredCameraLocation = TargetLocation - Offset;
		DesiredCameraLocation.Z = TargetLocation.Z + CameraHeightOffset;

		// --- Comprobación de colisión para evitar aparecer dentro de paredes ---
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		if (TargetActor)
		{
			Params.AddIgnoredActor(TargetActor);
		}

		bool bHit = GetWorld() && GetWorld()->LineTraceSingleByChannel(
			HitResult,
			TargetLocation,
			DesiredCameraLocation,
			ECC_GameTraceChannel2, // <-- usa tu canal de bloqueos
			Params
		);

		FVector FinalCameraLocation = DesiredCameraLocation;
		if (bHit)
		{
			FinalCameraLocation = HitResult.Location + HitResult.Normal * 10.0f;
		}

		FollowCamera->SetWorldLocation(FinalCameraLocation);

		// Rotación: mira al objetivo y aplica el pitch de la órbita
		FRotator CameraRot = (TargetLocation - FinalCameraLocation).Rotation();
		CameraRot.Pitch = OrbitPitch;
		FollowCamera->SetWorldRotation(CameraRot);
	}
}


void AWallECamera::AddOrbitInput(FVector2D Input)
{
	if (activeCamera == false) return;
	OrbitYaw += Input.X * 2.0f; // Orbita horizontal (mueve la cámara alrededor del objetivo)
	OrbitPitch = FMath::Clamp(OrbitPitch - Input.Y * 0.5f, MinPitch, MaxPitch); // Solo afecta la rotación local
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
	Offset.Z = 0.0f; // No se usa para la posición, solo para la rotación

	// La posición de la cámara es alrededor del objetivo, pero con altura fija
	FVector DesiredCameraLocation = TargetLocation - Offset;
	DesiredCameraLocation.Z = TargetLocation.Z + CameraHeightOffset;

	// --- Comprobación de colisión para evitar atravesar paredes ---
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
		ECC_GameTraceChannel2, // <-- Cambia a ECC_GameTraceChannelX que corresponda a tu WalleCameraBlock
		Params
	);

	FVector FinalCameraLocation = DesiredCameraLocation;
	if (bHit)
	{
		FinalCameraLocation = HitResult.Location + HitResult.Normal * 10.0f;
	}

	FollowCamera->SetWorldLocation(FinalCameraLocation);

	// Solo rota la cámara sobre su propio eje Y (pitch local)
	FRotator CameraRot = (TargetLocation - FinalCameraLocation).Rotation();
	CameraRot.Pitch = OrbitPitch; // Aplica el pitch solo a la rotación
	FollowCamera->SetWorldRotation(CameraRot);

	// Opcional: dibuja el trazo para depuración
	// DrawDebugLine(GetWorld(), TargetLocation, FinalCameraLocation, FColor::Red, false, 0.1f, 0, 2.0f);
}



