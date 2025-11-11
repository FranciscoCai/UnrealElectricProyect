// Fill out your copyright notice in the Description page of Project Settings.

#include "MainCamera.h"
#include "Camera/CameraComponent.h"
#include "Components/SplineComponent.h"
#include "Ea_ElectricProyectPlayerController.h"
#include "Components/SceneComponent.h" // Necesario para USceneComponent

// Sets default values
AMainCamera::AMainCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Crear y asignar el RootComponent explícitamente
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));


    // Adjuntar la cámara y el spline al RootComponent
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AMainCamera::BeginPlay()
{
	Super::BeginPlay();
    CachedPlayerController = GetWorld()->GetFirstPlayerController();
}

// Called every frame
void AMainCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (bIsTransitioning && cameraToChange && cameraToChange->FollowCamera)
    {
        ElapsedTransitionTime += DeltaTime;
        float Alpha = FMath::Clamp(ElapsedTransitionTime / TransitionDuration, 0.0f, 1.0f);
        float SmoothAlpha = FMath::SmoothStep(0.0f, 1.0f, Alpha);

        FVector TargetLocation = cameraToChange->FollowCamera->GetComponentLocation();
        FRotator TargetRotation = cameraToChange->FollowCamera->GetComponentRotation();

        FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, SmoothAlpha);
        FRotator NewRotation = FMath::Lerp(StartRotation, TargetRotation, SmoothAlpha);

        FollowCamera->SetWorldLocation(NewLocation);
        FollowCamera->SetWorldRotation(NewRotation);

        if (Alpha >= 1.0f)
        {
            bIsTransitioning = false;
            FollowCamera->SetWorldLocation(TargetLocation);
            FollowCamera->SetWorldRotation(TargetRotation);
            cameraToChange->OnActivatedCamera();
			cameraToChange = nullptr;
        }
    }
}

void AMainCamera::OnActivatedCamera()
{
    activeCamera = true;
    if (CachedPlayerController)
    {
        // Cast seguro al tipo correcto
        AEa_ElectricProyectPlayerController* MyController = Cast<AEa_ElectricProyectPlayerController>(CachedPlayerController);
        if (MyController)
        {
            MyController->CameraActor = this;
        }
        CachedPlayerController->SetViewTarget(this);
    }
}

void AMainCamera::OnUnActivatedCamera()
{
    activeCamera = false; // Modifica la variable miembro correctamente
}

void AMainCamera::StartCameraTransition()
{
    if (cameraToChange)
    {
        bIsTransitioning = true;
        StartLocation = FollowCamera->GetComponentLocation();
        StartRotation = FollowCamera->GetComponentRotation();
        ElapsedTransitionTime = 0.0f;
    }
}

