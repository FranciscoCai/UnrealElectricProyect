// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricLookAtCamera.h"
#include "Ea_ElectricProyectPlayerController.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"


// Sets default values
AElectricLookAtCamera::AElectricLookAtCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AElectricLookAtCamera::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC)
	{
		return;
	}
	AEa_ElectricProyectPlayerController* MyPC = Cast<AEa_ElectricProyectPlayerController>(PC);
	if (MyPC)
	{
		MyPC->LookAtOn = this;
	}
}

// Called every frame
void AElectricLookAtCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

