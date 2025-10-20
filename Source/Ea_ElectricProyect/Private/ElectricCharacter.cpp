// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricCharacter.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "ElectricSpline.h"
#include "ElectricPanel.h" 
#include "ElectricPanelStation.h" 
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

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
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PC)
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* LocalSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				// Aseg¨²rate de tener una variable UPROPERTY para tu MappingContext
				LocalSubsystem->AddMappingContext(ElectricMoveMappingContext, 0);
			}
		}
	}
}

// Called every frame
void AElectricCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveDirection != 0.f && SplineToFollow)
	{
		float Direction = MoveDirection;
		// Si bReverseSplineDirection es true (positivo), invierte la direcci¨®n
		if (bReverseSplineDirection)
		{
			Direction *= -1.f;
		}

		const float SplineLength = SplineToFollow->GetSplineLength();
		float Delta = SplineMoveSpeed * DeltaTime * Direction;
		CurrentDistanceOnSpline += Delta;

		if (CurrentDistanceOnSpline > SplineLength)
		{
			CurrentDistanceOnSpline = SplineLength;
			MoveDirection = 0.f;
			GetTargetpanel(1);
		}
		else if (CurrentDistanceOnSpline < 0.f)
		{
			CurrentDistanceOnSpline = 0.f;
			MoveDirection = 0.f;
			GetTargetpanel(0);
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
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(MoveUpAction, ETriggerEvent::Started, this, &AElectricCharacter::OnInputGoUp);
		EnhancedInput->BindAction(MoveDownAction, ETriggerEvent::Started, this, &AElectricCharacter::OnInputBackUp);
		EnhancedInput->BindAction(MoveUpAction, ETriggerEvent::Canceled, this, &AElectricCharacter::OnInputGoDown);
		EnhancedInput->BindAction(MoveDownAction, ETriggerEvent::Canceled, this, &AElectricCharacter::OnInputBackDown);
	}
}

void AElectricCharacter::SetSpline(USplineComponent* InSpline, float InitialDistance)
{
	SplineToFollow = InSpline;
	CurrentDistanceOnSpline = InitialDistance;
	bMovingOnSpline = (SplineToFollow != nullptr);

	if (SplineToFollow)
	{
		FVector StartLocation = SplineToFollow->GetLocationAtDistanceAlongSpline(InitialDistance, ESplineCoordinateSpace::World);
		FRotator StartRotation = SplineToFollow->GetRotationAtDistanceAlongSpline(InitialDistance, ESplineCoordinateSpace::World);
		SetActorLocationAndRotation(StartLocation, StartRotation);
	}
}



//UE_LOG(LogTemp, Warning, TEXT("2222"));



void AElectricCharacter::OnInputGoUp()
{
    MoveDirection = 1.f; // Mover hacia adelante
}

void AElectricCharacter::OnInputBackUp()
{
    MoveDirection = -1.f; // Mover hacia atr¨¢s
}
void AElectricCharacter::OnInputGoDown()
{
	if (MoveDirection > 0)
	{
		MoveDirection = 0;
    }
}

void AElectricCharacter::OnInputBackDown()
{
	if (MoveDirection < 0)
	{
		MoveDirection = 0;
	}
}
void AElectricCharacter::UnPossessed()
{
}

void AElectricCharacter::GetTargetpanel(int endStart)
{

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (SplineToFollow)
	{
		if (AActor* SplineOwner = SplineToFollow->GetOwner())
		{
			if (AElectricSpline* ElectricSplineOwner = Cast<AElectricSpline>(SplineOwner))
			{
				if (endStart == 1)
				{
					AElectricPanelStation* EndStation = ElectricSplineOwner->PanelStationEnd;
					if (EndStation)
					{
						AElectricPanel* TargetPanel = EndStation->ElectricPanelClass;
						if (TargetPanel)
						{
							if (PC && PC->GetPawn() != TargetPanel)
							{
								PC->Possess(TargetPanel);
								Destroy();
							}
						}
						else
						{
						}
					}
					else
					{
					}
				}
				else if (endStart == 0)
				{
					AElectricPanelStation* StartStation = ElectricSplineOwner->PanelStationStart;
					if (StartStation)
					{
						AElectricPanel* TargetPanel = StartStation->ElectricPanelClass;
						if (TargetPanel)
						{
							if (PC && PC->GetPawn() != TargetPanel)
							{
								PC->Possess(TargetPanel);
								Destroy();
							}
						}
						else
						{
						}
					}
					else
					{
					}
				}
			}
			else
			{
			}
		}
	}
}

