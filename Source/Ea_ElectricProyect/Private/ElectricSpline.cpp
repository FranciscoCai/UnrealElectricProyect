// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricSpline.h"
#include "Components/SplineComponent.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

// Sets default values
AElectricSpline::AElectricSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;
}

// Called when the game starts or when spawned
void AElectricSpline::BeginPlay()
{
	Super::BeginPlay();
}

void AElectricSpline::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AElectricSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AElectricSpline::OpenClose(bool bIsOpen)
{
	UE_LOG(LogTemp, Log, TEXT("%s::OpenClose -> %s"), *GetName(), bIsOpen ? TEXT("Open") : TEXT("Close"));
}

