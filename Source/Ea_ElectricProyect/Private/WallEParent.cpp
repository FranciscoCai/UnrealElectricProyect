// Fill out your copyright notice in the Description page of Project Settings.

#include "WallEParent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Cameras/MainCamera.h"
#include "Panels/ElectricPanel_RobotStation.h" // Asegur¨¢te de incluir la cabecera
#include "Panels/ElectricPanelPickable.h"
#include "Components/PrimitiveComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Panels/ElectricPanelStation.h"
#include "TimerManager.h"
#include "Ea_ElectricProyectPlayerController.h"
// Sets default values
AWallEParent::AWallEParent()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HeldPanel = nullptr;
	PickBoxComp = nullptr;
}

// Called when the game starts or when spawned
void AWallEParent::BeginPlay()
{
	Super::BeginPlay();

	// Try to find a component named or tagged "PickBox" on this actor (Blueprint BoxCollision)
	TArray<UActorComponent*> Comps;
	GetComponents(Comps);
	for (UActorComponent* Comp : Comps)
	{
		if (!Comp) continue;

		// Look for a component whose name contains "PickBox" or has component tag "PickBox"
		if (Comp->GetName().Contains(TEXT("PickBox")) || Comp->ComponentHasTag(TEXT("PickBox")))
		{
			PickBoxComp = Cast<UPrimitiveComponent>(Comp);
			break;
		}
	}

	// UE_LOG to confirm presence/absence of PickBox
	if (PickBoxComp)
	{
		UE_LOG(LogTemp, Log, TEXT("AWallEParent: PickBox found -> Name: %s, Class: %s"), *PickBoxComp->GetName(), *PickBoxComp->GetClass()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("daaadaa"), *GetName());
	}
}

// Called every frame
void AWallEParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AWallEParent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWallEParent::Move);
			// Nuevo binding para poner MovementSpeed a cero al soltar el bot¨®n
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Completed, this, &AWallEParent::OnMoveReleased);
		}
		if (InteractStation)
		{
			EnhancedInput->BindAction(InteractStation, ETriggerEvent::Started, this, &AWallEParent::BeginInteractStation);
			EnhancedInput->BindAction(InteractStation, ETriggerEvent::Canceled, this, &AWallEParent::CancelInteractStation);
		}
		if (PickUp)
		{
			EnhancedInput->BindAction(PickUp, ETriggerEvent::Started, this, &AWallEParent::OnPickUpOrDownStarted);
		}
		if (PressE)
		{
			EnhancedInput->BindAction(PressE, ETriggerEvent::Started, this, &AWallEParent::OnPressE);
		}
		if (PressQ)
		{
			EnhancedInput->BindAction(PressQ, ETriggerEvent::Started, this, &AWallEParent::OnPressQ);
		}
	}
}

void AWallEParent::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	bIsHoldingInteractStation = false;
	CurrentInteractStationHoldTime = 0.0f;
	bHasTriggeredInteractStation = false;
	// Apply mapping context when possessed by a local player controller
	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* LocalSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (WallEMappingContext)
				{
					LocalSubsystem->AddMappingContext(WallEMappingContext, 0);
				}
			}
		}
	}
}

void AWallEParent::UnPossessed()
{
	// Cache controller before calling Super because Super::UnPossessed may change it
	APlayerController* PC = Cast<APlayerController>(GetController());

	Super::UnPossessed();

	if (!PC) return;
	if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* LocalSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (WallEMappingContext)
			{
				LocalSubsystem->RemoveMappingContext(WallEMappingContext);
			}
		}
	}

	// Ensure any pending interact hold is cancelled on unpossess
	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(InteractStationTimerHandle);
	}
	bIsHoldingInteractStation = false;
	CurrentInteractStationHoldTime = 0.0f;
	bHasTriggeredInteractStation = false;
}

void AWallEParent::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// Calcula la magnitud de la velocidad de movimiento
	MovementSpeed = MovementVector.Size();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AWallEParent::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		FRotator Rotation;

		// Prefer camera rotation from the current view target if possible
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			if (AActor* ViewTarget = PC->GetViewTarget())
			{
				if (AMainCamera* CamActor = Cast<AMainCamera>(ViewTarget))
				{
					if (CamActor->FollowCamera)
					{
						Rotation = CamActor->FollowCamera->GetComponentRotation();
					}
					else
					{
						Rotation = PC->GetControlRotation();
					}
				}
				else
				{
					Rotation = PC->GetControlRotation();
				}
			}
			else
			{
				Rotation = PC->GetControlRotation();
			}
		}
		else
		{
			Rotation = GetController()->GetControlRotation();
		}

		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);

		// --- Solo el mesh visual apunta hacia la dirección de movimiento ---
		FVector MoveDir = (ForwardDirection * Forward) + (RightDirection * Right);
		if (!MoveDir.IsNearlyZero())
		{
			FRotator MeshTargetRotation = MoveDir.Rotation();
			MeshTargetRotation.Pitch = 0.f;
			MeshTargetRotation.Roll = 0.f;
			GetMesh()->SetWorldRotation(MeshTargetRotation);
		}
	}
}

void AWallEParent::BeginInteractStation()
{
	// If duration is zero or we're already holding and triggered, call immediately
	if (InteractStationHoldDuration <= 0.0f)
	{
		OnInteractStation();
		return;
	}

	if (GetWorld())
	{
		// Setup hold state and timer
		bIsHoldingInteractStation = true;
		bHasTriggeredInteractStation = false;
		CurrentInteractStationHoldTime = 0.0f;

		GetWorldTimerManager().ClearTimer(InteractStationTimerHandle);
		GetWorldTimerManager().SetTimer(InteractStationTimerHandle, [this]()
		{
			// Timer lambda runs on timer thread but in UE it's safe to call onto UObject as long as world exists.
			// We route to member function to keep logic consistent.
			if (!bHasTriggeredInteractStation)
			{
				bHasTriggeredInteractStation = true;
				bIsHoldingInteractStation = false;
				CurrentInteractStationHoldTime = InteractStationHoldDuration;
				OnInteractStation();
			}
		}, InteractStationHoldDuration, false);
	}
}

void AWallEParent::CancelInteractStation()
{
	// Clear timer and reset state
	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(InteractStationTimerHandle);
	}
	bIsHoldingInteractStation = false;
	CurrentInteractStationHoldTime = 0.0f;
	bHasTriggeredInteractStation = false;
}

void AWallEParent::OnInteractStation()
{
	// Default behavior when hold completes: possess the RobotStationRef (same logic as previous instant version)
	if (RobotStationRef)
	{
		if (RobotStationRef && RobotStationRef->CameraToGo && RobotStationRef->CameraToGo->bIsTransitioning)
		{
			return; 
		}
		if (AEa_ElectricProyectPlayerController* PC = Cast<AEa_ElectricProyectPlayerController>(GetController()))
		{
			PC->ChangeCameraStart(RobotStationRef->CameraToBack, RobotStationRef->BackDuration);
			PC->Possess(RobotStationRef);
		}
	}
}

void AWallEParent::OnPickUpOrDownStarted()
{
	if (HeldPanel)
	{
		// Si ya est¨¢ sosteniendo un panel, suelta
		OnPickDownStarted();
	}
	else
	{
		// Si no est¨¢ sosteniendo nada, recoge
		OnPickUpStarted();
	}
}

void AWallEParent::OnPickUpStarted()
{
	// If already holding something, ignore
	if (HeldPanel) return;
	// Ensure we have PickBox component cached, if not attempt to find it now
	if (!PickBoxComp)
	{
		TArray<UActorComponent*> Comps;
		GetComponents(Comps);
		for (UActorComponent* Comp : Comps)
		{
			if (!Comp) continue;
			if (Comp->GetName().Contains(TEXT("PickBox")) || Comp->ComponentHasTag(TEXT("PickBox")))
			{
				PickBoxComp = Cast<UPrimitiveComponent>(Comp);
				break;
			}
		}
	}
	if (!PickBoxComp) return;
	// Get overlapping actors of the pickbox and find an ElectricPanel
	TArray<AActor*> OverlapActors;
	PickBoxComp->GetOverlappingActors(OverlapActors, AElectricPanelPickable::StaticClass());
	if (OverlapActors.Num() == 0) return;

	AElectricPanelPickable* Panel = nullptr;
	for (AActor* Actor : OverlapActors)
	{
		if (!Actor) continue;
		if (AElectricPanelPickable* P = Cast<AElectricPanelPickable>(Actor))
		{
			if (P->CanPick)
			{
				Panel = P;
				break;
			}
		}
	}

	if (!Panel) return;
	// If the panel was placed on a station, clear the station's reference to it
	if (Panel->ElectricPanelStationOn)
	{
		AElectricPanelStation* PanelStationOn = Panel->ElectricPanelStationOn;
		// Clear station's stored panel pointer so station no longer references this panel
		PanelStationOn->ElectricPanelClass = nullptr;
		// Update splines so station state is consistent
		PanelStationOn->UpdateSplinesPanelStation();
		// Clear panel's back-reference
		Panel->ElectricPanelStationOn = nullptr;
	}

	// Attach the panel to the PickBox component so it follows its position continually
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	Panel->AttachToComponent(PickBoxComp, AttachRules);

	// Optionally, disable physics or movement on the panel here if needed (not done by default)

	HeldPanel = Panel;
}

void AWallEParent::OnPickDownStarted()
{
	if (!HeldPanel) return;

	HeldPanel->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// Ensure PickBoxComp exists (try to find again if needed)
	if (!PickBoxComp)
	{
		TArray<UActorComponent*> Comps;
		GetComponents(Comps);
		for (UActorComponent* Comp : Comps)
		{
			if (!Comp) continue;
			if (Comp->GetName().Contains(TEXT("PickBox")) || Comp->ComponentHasTag(TEXT("PickBox")))
			{
				PickBoxComp = Cast<UPrimitiveComponent>(Comp);
				break;
			}
		}
	}

	// If there's no PickBox component, just drop the panel and exit safely
	if (!PickBoxComp)
	{
		HeldPanel = nullptr;
		return;
	}

	// Get overlapping station actors safely and check array size before access
	TArray<AActor*> OverlapActors;
	PickBoxComp->GetOverlappingActors(OverlapActors, AElectricPanelStation::StaticClass());

	if (OverlapActors.Num() == 0)
	{
		// No station overlapped: leave the panel where it was detached
		HeldPanel = nullptr;
		return;
	}

	AElectricPanelStation* Station = Cast<AElectricPanelStation>(OverlapActors[0]);

	if (Station && HeldPanel && !IsValid(Station->ElectricPanelClass))
	{
		// Snap the held panel to the station's world transform (position + rotation)
		HeldPanel->SetActorLocationAndRotation(Station->GetActorLocation(), Station->GetActorRotation());
		Station -> SetelectricPanelInformation(HeldPanel);
		Station->SetelectricStationPanelInformation(HeldPanel);
		Station->UpdateSplinesPanelStation();
		if (AElectricPanelPickable* HeldPanelPickable = Cast<AElectricPanelPickable>(HeldPanel))
		{
			HeldPanelPickable->OnSnapPanelEvent();
		}

	}

	HeldPanel = nullptr;
}

void AWallEParent::OnPressE()
{
}

void AWallEParent::OnPressQ()
{
}

void AWallEParent::OnMoveReleased(const FInputActionValue& Value)
{
	MovementSpeed = 0.0f;
}

