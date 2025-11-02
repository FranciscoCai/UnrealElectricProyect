// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricPanel.h"
#include "ElectricPanelStation.h"
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

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && PC->GetPawn() != this && poseOnStart)
	{
		PC->Possess(this);
	}
	if (PC && PC->GetPawn() == this)
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* LocalSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (PanelMappingContext)
				{
					LocalSubsystem->AddMappingContext(PanelMappingContext, 0);
				}
			}
		}
	}
}

void AElectricPanel::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Apply the panel's mapping context every time this pawn is possessed by a local player controller.
	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* LocalSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (PanelMappingContext)
				{
					LocalSubsystem->AddMappingContext(PanelMappingContext, 0);
				}
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
		EnhancedInput->BindAction(MoveRightAction, ETriggerEvent::Started, this, &AElectricPanel::OnInputRight);
		EnhancedInput->BindAction(MoveLeftAction, ETriggerEvent::Started, this, &AElectricPanel::OnInputLeft);
		EnhancedInput->BindAction(Interact, ETriggerEvent::Started, this, &AElectricPanel::OnInteract);
	}
}


void AElectricPanel::OnInputUp()    { SpawnAndPossessCharacter(0, ElectricPanelStationOn->bSpawnUp); }
void AElectricPanel::OnInputLeft()  { SpawnAndPossessCharacter(1, ElectricPanelStationOn->bSpawnLeft); }
void AElectricPanel::OnInputDown() { SpawnAndPossessCharacter(2, ElectricPanelStationOn->bSpawnDown); }
void AElectricPanel::OnInputRight()  { SpawnAndPossessCharacter(3, ElectricPanelStationOn->bSpawnRight); }
void AElectricPanel::OnInteract()  { ; }

void AElectricPanel::SpawnAndPossessCharacter(int32 SplineIndex, bool bSpawnAtStart)
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
	if (!ElectricCharacterClass || !ElectricPanelStationOn->Splines.IsValidIndex(SplineIndex) || !ElectricPanelStationOn->Splines[SplineIndex])
		return;
	AElectricSpline* SplineActor = ElectricPanelStationOn->Splines[SplineIndex];
	USplineComponent* SplineComp = SplineActor->FindComponentByClass<USplineComponent>();
	if (!SplineComp) return;

	float InitialDistance = bSpawnAtStart ? 0.0f : SplineComp->GetSplineLength();
	FVector SpawnLocation = SplineComp->GetLocationAtDistanceAlongSpline(InitialDistance, ESplineCoordinateSpace::World);
	FRotator SpawnRotation = SplineComp->GetRotationAtDistanceAlongSpline(InitialDistance, ESplineCoordinateSpace::World);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AElectricCharacter* NewCharacter = GetWorld()->SpawnActor<AElectricCharacter>(ElectricCharacterClass, SpawnLocation, SpawnRotation, SpawnParams);
				
		if (NewCharacter)
		{
			NewCharacter->SetSpline(SplineComp, InitialDistance);
			NewCharacter->bReverseSplineDirection = !bSpawnAtStart;

			APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC)
			{
				PC->Possess(NewCharacter);
			}
		}
}

void AElectricPanel::UnPossessed()
{
	// Guardar el Controller antes de llamar a Super, ya que Super::UnPossessed()
	// puede limpiar el Controller o ejecutar el evento Blueprint en los hijos.
	APlayerController* PC = Cast<APlayerController>(GetController());

	// Llamar a la implementación base para permitir que los BPs hijos reciban el Event UnPossessed.
	Super::UnPossessed();

	if (!PC) return;

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer) return;

	UEnhancedInputLocalPlayerSubsystem* LocalSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (LocalSubsystem && PanelMappingContext)
	{
		LocalSubsystem->RemoveMappingContext(PanelMappingContext);
	}
}

