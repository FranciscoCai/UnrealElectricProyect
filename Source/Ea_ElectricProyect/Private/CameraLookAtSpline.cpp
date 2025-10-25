	// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraLookAtSpline.h"
#include "Components/SplineComponent.h"

// Sets default values
ACameraLookAtSpline::ACameraLookAtSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Crear y adjuntar el componente spline
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;
}

// Called when the game starts or when spawned
void ACameraLookAtSpline::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACameraLookAtSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

