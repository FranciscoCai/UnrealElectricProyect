// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricCamera.h"

// Sets default values
AElectricCamera::AElectricCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AElectricCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AElectricCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

