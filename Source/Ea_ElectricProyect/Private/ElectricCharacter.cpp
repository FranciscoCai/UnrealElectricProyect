// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricCharacter.h"
#include "Components/SplineComponent.h"

// Sets default values
AElectricCharacter::AElectricCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AElectricCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AElectricCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bMovingOnSpline && SplineToFollow)
	{
		const float SplineLength = SplineToFollow->GetSplineLength();
		float Delta = SplineMoveSpeed * DeltaTime;
		if (bReverseSplineDirection)
		{
			Delta = -Delta;
		}
		CurrentDistanceOnSpline += Delta;

		// Limitar el rango
		if (CurrentDistanceOnSpline > SplineLength)
		{
			CurrentDistanceOnSpline = SplineLength;
			bMovingOnSpline = false;
		}
		else if (CurrentDistanceOnSpline < 0.f)
		{
			CurrentDistanceOnSpline = 0.f;
			bMovingOnSpline = false;
		}

		FVector NewLocation = SplineToFollow->GetLocationAtDistanceAlongSpline(CurrentDistanceOnSpline, ESplineCoordinateSpace::World);
		FRotator NewRotation = SplineToFollow->GetRotationAtDistanceAlongSpline(CurrentDistanceOnSpline, ESplineCoordinateSpace::World);

		SetActorLocationAndRotation(NewLocation, NewRotation);
	}
}

// Called to bind functionality to input
void AElectricCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AElectricCharacter::SetSpline(USplineComponent* InSpline, float InitialDistance)
{
	SplineToFollow = InSpline;
	UE_LOG(LogTemp, Warning, TEXT("InitialDistance: %f"), InitialDistance);
	CurrentDistanceOnSpline = InitialDistance;
	bMovingOnSpline = (SplineToFollow != nullptr);

	if (SplineToFollow)
	{
		FVector StartLocation = SplineToFollow->GetLocationAtDistanceAlongSpline(InitialDistance, ESplineCoordinateSpace::World);
		FRotator StartRotation = SplineToFollow->GetRotationAtDistanceAlongSpline(InitialDistance, ESplineCoordinateSpace::World);
		SetActorLocationAndRotation(StartLocation, StartRotation);
	}
}

