// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricPanel.h"
#include "ElectricCharacter.h"
#include "ElectricSpline.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Components/SplineComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Logging/LogMacros.h"
#include "Engine/Engine.h"
// Sets default values
AElectricPanel::AElectricPanel()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AElectricPanel::BeginPlay()
{
	Super::BeginPlay();

	// Poseer el panel como antes
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && PC->GetPawn() != this)
	{
		PC->Possess(this);
	}

	// Asignar el Mapping Context
	if (PC)
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* LocalSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				// Asegúrate de tener una variable UPROPERTY para tu MappingContext
				LocalSubsystem->AddMappingContext(PanelMappingContext, 0);
			}
		}
	}
}

// Called every frame
void AElectricPanel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AElectricPanel::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(MoveUpAction, ETriggerEvent::Started, this, &AElectricPanel::OnInputUp);
		EnhancedInput->BindAction(MoveDownAction, ETriggerEvent::Started, this, &AElectricPanel::OnInputDown);
		EnhancedInput->BindAction(MoveLeftAction, ETriggerEvent::Started, this, &AElectricPanel::OnInputLeft);
		EnhancedInput->BindAction(MoveRightAction, ETriggerEvent::Started, this, &AElectricPanel::OnInputRight);
	}
}


void AElectricPanel::OnInputUp()    { SpawnAndPossessCharacter(0); }
void AElectricPanel::OnInputDown()  { SpawnAndPossessCharacter(1); }
void AElectricPanel::OnInputLeft()  { SpawnAndPossessCharacter(2); }
void AElectricPanel::OnInputRight() { SpawnAndPossessCharacter(3); }

void AElectricPanel::SpawnAndPossessCharacter(int32 SplineIndex)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, // Key (-1 para que cada mensaje sea único)
			5.0f, // Duración en segundos
			FColor::Yellow, // Color del texto
			TEXT("¡PRUEBA DE LOG EN PANTALLA!")
		);
	}
	if (!ElectricCharacterClass || !Splines.IsValidIndex(SplineIndex) || !Splines[SplineIndex])
		return;
	AElectricSpline* SplineActor = Splines[SplineIndex];
	USplineComponent* SplineComp = SplineActor->FindComponentByClass<USplineComponent>();
	if (!SplineComp) return;

	// Elegir el punto: 0 para inicio, último índice para final
	int32 PointIndex = bSpawnAtEnd ? SplineComp->GetNumberOfSplinePoints() - 1 : 0;
	FVector SpawnLocation = SplineComp->GetLocationAtSplinePoint(PointIndex, ESplineCoordinateSpace::World);
	FRotator SpawnRotation = SplineComp->GetRotationAtSplinePoint(PointIndex, ESplineCoordinateSpace::World);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	AElectricCharacter* NewCharacter = GetWorld()->SpawnActor<AElectricCharacter>(ElectricCharacterClass, SpawnLocation, SpawnRotation, SpawnParams);

	float InitialDistance = 1.0f;
	if (bSpawnAtEnd && SplineComp)
	{
		InitialDistance = SplineComp->GetSplineLength();
		NewCharacter->bReverseSplineDirection = true;
	}
	NewCharacter->SetSpline(SplineComp, InitialDistance);

	if (NewCharacter)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			UnPossessed();
			PC->Possess(NewCharacter);
		}
	}
}

void AElectricPanel::UnPossessed()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer) return;

	UEnhancedInputLocalPlayerSubsystem* LocalSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (LocalSubsystem && PanelMappingContext)
	{
		LocalSubsystem->RemoveMappingContext(PanelMappingContext);
	}
}

