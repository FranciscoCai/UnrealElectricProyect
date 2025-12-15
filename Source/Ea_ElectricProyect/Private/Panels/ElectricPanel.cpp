// Fill out your copyright notice in the Description page of Project Settings.


#include "Panels/ElectricPanel.h"
#include "Panels/ElectricPanelStation.h"
#include "ElectricCharacter.h"
#include "ElectricSpline.h"
#include "Cameras/CameraLookAtSpline.h"
#include "Ea_ElectricProyectPlayerController.h"
#include "Cameras/ElectricLookAtCamera.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Components/SplineComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Logging/LogMacros.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

// Sets default values
AElectricPanel::AElectricPanel()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if no other logic necesita Tick.
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
}

void AElectricPanel::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// Asegurar estado limpio al poseer
	bIsHoldingInteract = false;
	CurrentInteractHoldTime = 0.0f;
	bHasTriggeredInteract = false;
	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(InteractTimerHandle);
	}

	// Apply the panel's mapping context every time this pawn is possessed by a local player controller.
	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* LocalSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (AEa_ElectricProyectPlayerController* EaPC = Cast<AEa_ElectricProyectPlayerController>(PC))
				{
					if (EaPC)
					{
						if (EaPC->ElectricMoveMappingContext)
						{
							LocalSubsystem->AddMappingContext(EaPC->ElectricMoveMappingContext, 0);
						}
						if (EaPC->PanelMappingContext)
						{
							LocalSubsystem->AddMappingContext(EaPC->PanelMappingContext, 1);
						}
					}
				}
			}
		}
	}
}

// Called every frame
void AElectricPanel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{

		PC->PlayDynamicForceFeedback(0.0f, 0.0f, true, true, true, true, EDynamicForceFeedbackAction::Stop);
	}
	// Nota: ahora el hold-to-interact NO depende de Tick.
	// Si necesitas mostrar una barra de progreso en UI actualízala desde el Widget (binding) o usa un pequeño timer que actualice CurrentInteractHoldTime si lo deseas.
}

// Called to bind functionality to input
void AElectricPanel::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	AEa_ElectricProyectPlayerController* EaPC = Cast<AEa_ElectricProyectPlayerController>(GetController());
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if(EaPC)
		{
			// Start directional hold on Started, cancel on Canceled
			EnhancedInput->BindAction(EaPC->MoveUpAction, ETriggerEvent::Started, this, &AElectricPanel::OnInputUp);
			EnhancedInput->BindAction(EaPC->MoveUpAction, ETriggerEvent::Canceled, this, &AElectricPanel::OnInputUpReleased);

			EnhancedInput->BindAction(EaPC->MoveLeftAction, ETriggerEvent::Started, this, &AElectricPanel::OnInputLeft);
			EnhancedInput->BindAction(EaPC->MoveLeftAction, ETriggerEvent::Canceled, this, &AElectricPanel::OnInputLeftReleased);

			EnhancedInput->BindAction(EaPC->MoveDownAction, ETriggerEvent::Started, this, &AElectricPanel::OnInputDown);
			EnhancedInput->BindAction(EaPC->MoveDownAction, ETriggerEvent::Canceled, this, &AElectricPanel::OnInputDownReleased);

			EnhancedInput->BindAction(EaPC->MoveRightAction, ETriggerEvent::Started, this, &AElectricPanel::OnInputRight);
			EnhancedInput->BindAction(EaPC->MoveRightAction, ETriggerEvent::Canceled, this, &AElectricPanel::OnInputRightReleased);
		}

		// Bind de Interact: Started -> BeginInteract, Canceled -> CancelInteract
		EnhancedInput->BindAction(Interact, ETriggerEvent::Started, this, &AElectricPanel::BeginInteract);
		EnhancedInput->BindAction(Interact, ETriggerEvent::Canceled, this, &AElectricPanel::CancelInteract);
	}
}


void AElectricPanel::OnInputUp()
{
    if (!LimitUp) return;
    if (!ElectricPanelStationOn) return;
	CancelDirectionalHold();

		PendingDirectionalControlIndex = 0;
		bPendingDirectionalSpawnAtStart = ElectricPanelStationOn->bSpawnUp;
		bPendingDirectionalReverseInput = false;
	

    bHasPendingDirectionalSpawn = true;

    if (GetWorld())
    {
		StartDirectionBP(PendingDirectionalControlIndex);
        GetWorldTimerManager().SetTimer(DirectionalHoldTimerHandle, this, &AElectricPanel::OnDirectionalHoldFinished, DirectionalHoldDuration, false);
    }
}

void AElectricPanel::OnInputLeft()
{
    if (!LimitLeft) return;
    if (!ElectricPanelStationOn) return;
	CancelDirectionalHold();
	if (ElectricPanelStationOn->isCelling)
	{
		PendingDirectionalControlIndex = 3;
	}
	else
	{
		PendingDirectionalControlIndex = 1;
	}


    bPendingDirectionalSpawnAtStart = ElectricPanelStationOn->bSpawnLeft;
    bPendingDirectionalReverseInput = true;
    bHasPendingDirectionalSpawn = true;

    if (GetWorld())
    {
		StartDirectionBP(PendingDirectionalControlIndex);
        GetWorldTimerManager().SetTimer(DirectionalHoldTimerHandle, this, &AElectricPanel::OnDirectionalHoldFinished, DirectionalHoldDuration, false);
    }
}

void AElectricPanel::OnInputDown()
{
    if (!LimitDown) return;
    if (!ElectricPanelStationOn) return;
	CancelDirectionalHold();

		PendingDirectionalControlIndex = 2;
		bPendingDirectionalSpawnAtStart = ElectricPanelStationOn->bSpawnDown;
		bPendingDirectionalReverseInput = true;

    bHasPendingDirectionalSpawn = true;

    if (GetWorld())
    {
		StartDirectionBP(PendingDirectionalControlIndex);
        GetWorldTimerManager().SetTimer(DirectionalHoldTimerHandle, this, &AElectricPanel::OnDirectionalHoldFinished, DirectionalHoldDuration, false);
    }
}

void AElectricPanel::OnInputRight()
{
    if (!LimitRight) return;
    if (!ElectricPanelStationOn) return;
	CancelDirectionalHold();
	if (ElectricPanelStationOn->isCelling)
	{
		PendingDirectionalControlIndex = 1;
	}
	else
	{
		PendingDirectionalControlIndex = 3;
	}


    bPendingDirectionalSpawnAtStart = ElectricPanelStationOn->bSpawnRight;
    bPendingDirectionalReverseInput = false;
    bHasPendingDirectionalSpawn = true;

    if (GetWorld())
    {
		StartDirectionBP(PendingDirectionalControlIndex);
        GetWorldTimerManager().SetTimer(DirectionalHoldTimerHandle, this, &AElectricPanel::OnDirectionalHoldFinished, DirectionalHoldDuration, false);
    }
}

void AElectricPanel::OnInputUpReleased()
{
    // Only cancel if the released key is the one currently pending
    if (bHasPendingDirectionalSpawn)
    {
		CancelDirectionalHold();
		bHasPendingDirectionalSpawn = false;
		PendingDirectionalControlIndex = -1;
    }
}

void AElectricPanel::OnInputLeftReleased()
{
    if (bHasPendingDirectionalSpawn)
    {
		CancelDirectionalHold();
		bHasPendingDirectionalSpawn = false;
		PendingDirectionalControlIndex = -1;
    }
}

void AElectricPanel::OnInputDownReleased()
{
    if (bHasPendingDirectionalSpawn)
    {
		CancelDirectionalHold();
		bHasPendingDirectionalSpawn = false;
		PendingDirectionalControlIndex = -1;
    }
}

void AElectricPanel::OnInputRightReleased()
{
    if (bHasPendingDirectionalSpawn)
    {
		CancelDirectionalHold();
		bHasPendingDirectionalSpawn = false;
		PendingDirectionalControlIndex = -1;
    }
}

void AElectricPanel::OnDirectionalHoldFinished()
{
	// ensure we still have a valid pending spawn
	if (!bHasPendingDirectionalSpawn || PendingDirectionalControlIndex < 0) return;

	int32 ControlIndex = PendingDirectionalControlIndex;
	if (ElectricPanelStationOn->isCelling && PendingDirectionalControlIndex ==1)
	{
		ControlIndex = 3;
	}
	else if (ElectricPanelStationOn->isCelling && PendingDirectionalControlIndex == 3)
	{
		ControlIndex = 1;
	}
	bool bSpawnAtStart = bPendingDirectionalSpawnAtStart;
	bool bReverseInput = bPendingDirectionalReverseInput;

	CancelDirectionalHold();
	bHasPendingDirectionalSpawn = false;
	PendingDirectionalControlIndex = -1;

	// perform spawn
	SpawnAndPossessCharacter(ControlIndex, bSpawnAtStart, bReverseInput);
}

void AElectricPanel::CancelDirectionalHold()
{
	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(DirectionalHoldTimerHandle);
	}


	if (PendingDirectionalControlIndex == 0) { CancelDirectionBP(0); }
	else if (PendingDirectionalControlIndex == 1) { CancelDirectionBP(1); }
	else if (PendingDirectionalControlIndex == 2) { CancelDirectionBP(2); }
	else if (PendingDirectionalControlIndex == 3) { CancelDirectionBP(3); }
}

void AElectricPanel::OnInteract()
{
	UE_LOG(LogTemp, Log, TEXT("AElectricPanel::OnInteract() - Interact completado"));
}

void AElectricPanel::BeginInteract()
{
	BeginInteractBP();
	// Si la duración es 0, ejecutar inmediatamente
	if (InteractHoldDuration <= 0.0f)
	{
		OnInteract();
		return;
	}

	// Iniciar timer que llamará a OnInteractTimerFinished
	if (GetWorld())
	{
		// Opcional: actualizar estado visible
		bIsHoldingInteract = true;
		bHasTriggeredInteract = false;
		CurrentInteractHoldTime = 0.0f; // si quieres mantener este valor, puedes iniciarlo aquí

		// Start vibration on controller (local player)
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			// Intensity [0..1], duration matches hold duration, affect all motors (set booleans as desired)
			PC->PlayDynamicForceFeedback(0.7f, InteractHoldDuration, true, true, true, true, EDynamicForceFeedbackAction::Start);
		}

		GetWorldTimerManager().ClearTimer(InteractTimerHandle);
		GetWorldTimerManager().SetTimer(InteractTimerHandle, this, &AElectricPanel::OnInteractTimerFinished, InteractHoldDuration, false);
	}
}

void AElectricPanel::CancelInteract()
{
	CancelInteractBP();
	// Limpiar timer y resetear estado
	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(InteractTimerHandle);
	}
	bIsHoldingInteract = false;
	CurrentInteractHoldTime = 0.0f;
	bHasTriggeredInteract = false;

	// Stop vibration if running
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->PlayDynamicForceFeedback(0.0f, 0.0f, true, true, true, true, EDynamicForceFeedbackAction::Stop);
	}
}

void AElectricPanel::OnInteractTimerFinished()
{
	FinishInteractBP();
	// Evitar dobles ejecuciones
	if (bHasTriggeredInteract) return;

	bHasTriggeredInteract = true;
	bIsHoldingInteract = false;
	CurrentInteractHoldTime = InteractHoldDuration;

	// Stop vibration
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->PlayDynamicForceFeedback(0.0f, 0.0f, true, true, true, true, EDynamicForceFeedbackAction::Stop);
	}

	// Llamada a la función que los hijos pueden sobreescribir
	OnInteract();
}

void AElectricPanel::SpawnAndPossessCharacter(int32 ControlIndex, bool bSpawnAtStart, bool bReverseInput)
{
	// Validate pointers/indices in a safe order to avoid dereferencing null.
	if (!ElectricCharacterClass || !ElectricPanelStationOn)
		return;
	if (!ElectricPanelStationOn->Splines.IsValidIndex(ControlIndex) || !ElectricPanelStationOn->Splines[ControlIndex])
		return;
	if(ElectricPanelStationOn->Splines[ControlIndex]->SplineActivatableType == true)
	{
		if(ElectricPanelStationOn->Splines[ControlIndex]->IsActive == false)
		{
			return;
		}
	}
	AElectricSpline* SplineActor = ElectricPanelStationOn->Splines[ControlIndex];
	USplineComponent* SplineComp = SplineActor->FindComponentByClass<USplineComponent>();
	if (!SplineComp) return;

	float InitialDistance = bSpawnAtStart ? 0.0f : SplineComp->GetSplineLength();
	FVector SpawnLocation = SplineComp->GetLocationAtDistanceAlongSpline(InitialDistance, ESplineCoordinateSpace::World);
	FRotator SpawnRotation = SplineComp->GetRotationAtDistanceAlongSpline(InitialDistance, ESplineCoordinateSpace::World);

	if (ElectricPanelStationOn->LookAtSplineChange.IsValidIndex(ControlIndex))
	{
		ACameraLookAtSpline* LookAtSplineActor = ElectricPanelStationOn->LookAtSplineChange[ControlIndex];
		if (LookAtSplineActor)
		{
			APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC)
			{
				AEa_ElectricProyectPlayerController *EaPC = Cast<AEa_ElectricProyectPlayerController>(PC);
				if (EaPC && EaPC->LookAtOn)
				{
					// Assign the actor (ACameraLookAtSpline*), not the USplineComponent*
					EaPC->LookAtOn->CameraLookAtSpline = LookAtSplineActor;
				}
			}
		}
	}

	if (!GetWorld()) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AElectricCharacter* NewCharacter = GetWorld()->SpawnActor<AElectricCharacter>(ElectricCharacterClass, SpawnLocation, SpawnRotation, SpawnParams);
	AEa_ElectricProyectPlayerController* EaPC = Cast<AEa_ElectricProyectPlayerController>(GetController());
	if (NewCharacter && EaPC)
	{
		NewCharacter->SetSpline(SplineComp, InitialDistance);
		NewCharacter->bReverseSplineDirection = !bSpawnAtStart;
		NewCharacter->bReverseInputDirection = bReverseInput;

		//if (ControlIndex == 0 || ControlIndex == 2) // Up or Down -> keep vertical mapping
		//{
		//	NewCharacter->MoveGoAction = EaPC->MoveUpAction;
		//	NewCharacter->MoveBackAction = EaPC->MoveDownAction;
		//}
		//else
		//{
		//	NewCharacter->MoveGoAction = EaPC->MoveRightAction;
		//	NewCharacter->MoveBackAction = EaPC->MoveLeftAction;
		//}

		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			poseElectricCharacter = true;
			PC->Possess(NewCharacter);
			if (ControlIndex == 0 || ControlIndex == 3)
			{
				NewCharacter->OnInputGoUp();
			}
			else if(ControlIndex == 1 || ControlIndex == 2)
			{
				NewCharacter->OnInputBackUp();
			}
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

	// Asegurarse de cancelar cualquier hold pendiente cuando nos despossesen
	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(InteractTimerHandle);
		GetWorldTimerManager().ClearTimer(DirectionalHoldTimerHandle);
	}
	bIsHoldingInteract = false;
	CurrentInteractHoldTime = 0.0f;
	bHasTriggeredInteract = false;
	bHasPendingDirectionalSpawn = false;
	PendingDirectionalControlIndex = -1;

	// Ensure vibration is stopped
	if (PC)
	{
		PC->PlayDynamicForceFeedback(0.0f, 0.0f, true, true, true, true, EDynamicForceFeedbackAction::Stop);
	}

	if (!PC) return;

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer) return;

	UEnhancedInputLocalPlayerSubsystem* LocalSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (LocalSubsystem)
	{
		if (AEa_ElectricProyectPlayerController* EaPC = Cast<AEa_ElectricProyectPlayerController>(PC))
		{
			if (EaPC)
			{
				if (EaPC->ElectricMoveMappingContext&& !poseElectricCharacter)
				{
					LocalSubsystem->RemoveMappingContext(EaPC->ElectricMoveMappingContext);
				}
				else
				{
					poseElectricCharacter = false;
				}
				if (EaPC->PanelMappingContext)
				{
					LocalSubsystem->RemoveMappingContext(EaPC->PanelMappingContext);
				}
			}
		}
	}
}

