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
#include "Cameras/WallECamera.h"
#include "Ea_ElectricProyectPlayerController.h"
// Sets default values
AWallEParent::AWallEParent()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HeldPanel = nullptr;

	// Desactiva la rotaci¨®n autom¨¢tica
	bUseControllerRotationYaw = false;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
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
}

// Called every frame
void AWallEParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CameraToChange)
	{
		if (!CameraToChange->bIsTransitioning)
		{

			if (UCameraComponent* FollowCam = CameraToChange->FindComponentByClass<UCameraComponent>())
			{
				FRotator CameraRot = FollowCam->GetComponentRotation();
				FRotator TargetRot(0.f, CameraRot.Yaw, 0.f);

				// Interpolaci¨®n suave
				CurrentInterpRotation = FMath::RInterpTo(CurrentInterpRotation, TargetRot, DeltaTime, 10.0f);
				SetActorRotation(CurrentInterpRotation);
			}
		}
	}
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
		if (WallELook)
		{
			EnhancedInput->BindAction(WallELook, ETriggerEvent::Triggered, this, &AWallEParent::OnLookInput);
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
    FVector2D MovementVector = Value.Get<FVector2D>();
    float ForwardValue = MovementVector.Y;
    float RightValue = MovementVector.X;

    // Actualiza la direcci¨®n de movimiento
    MovementDirection = MovementVector;

    MovementSpeed = FMath::Abs(ForwardValue) + FMath::Abs(RightValue);

    FRotator YawRot = GetActorRotation();
    FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

    if (!FMath::IsNearlyZero(ForwardValue))
    {
        AddMovementInput(ForwardDir, ForwardValue);
    }
    if (!FMath::IsNearlyZero(RightValue))
    {
        AddMovementInput(RightDir, RightValue);
    }
    // NO cambies la rotaci¨®n aqu¨ª
}

void AWallEParent::DoMove(float Right, float Forward)
{
	// ÒÑÆúÓÃ»òÁô¿Õ£¬·ÀÖ¹Á´½Ó´íÎó
}

void AWallEParent::BeginInteractStation()
{
	
	if (GetWorld())
	{
		if(RobotStationRef && RobotStationRef->CameraToBack)
		{
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

			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				// Intensity [0..1], duration matches hold duration, affect all motors (set booleans as desired)
				PC->PlayDynamicForceFeedback(0.7f, InteractHoldDuration, true, true, true, true, EDynamicForceFeedbackAction::Start);
			}
		}
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
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->PlayDynamicForceFeedback(0.0f, 0.0f, true, true, true, true, EDynamicForceFeedbackAction::Stop);
	}
}

void AWallEParent::OnInteractStation()
{
	// Default behavior when hold completes: possess the RobotStationRef (same logic as previous instant version)
	if (RobotStationRef)
	{
		if (RobotStationRef->CameraToGo->bIsTransitioning)
		{
			return; 
		}
		if (AEa_ElectricProyectPlayerController* PC = Cast<AEa_ElectricProyectPlayerController>(GetController()))
		{
			PC->ChangeCameraStart(RobotStationRef->CameraToBack, RobotStationRef->BackDuration);
			PC->Possess(RobotStationRef);
			RobotStationRef->WallEParentRef = this;
		}
	}
}

void AWallEParent::OnPickUpOrDownStarted()
{
	if(bPickTransition)
	{
		return;
	}
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
	PickBoxComp->GetOverlappingActors(OverlapActors, AElectricPanel::StaticClass());
	if (OverlapActors.Num() == 0) return;

	AElectricPanel* Panel = nullptr;
	for (AActor* Actor : OverlapActors)
	{
		if (!Actor) continue;
		if (AElectricPanel* P = Cast<AElectricPanel>(Actor))
		{
			Panel = P;
			break;
		}
	}

	if (Panel)
	{
		Panel->RobotInteractToPanel();
	}
	AElectricPanelPickable* pickablePanel = nullptr;
	for (AActor* Actor : OverlapActors)
	{
		if (!Actor) continue;
		if (AElectricPanelPickable* P = Cast<AElectricPanelPickable>(Actor))
		{
			if (P->CanPick)
			{
				pickablePanel = P;
				P->BP_OnPickPanelEvent();
				break;
			}
		}
	}

	if (pickablePanel)
	{
		// If the panel was placed on a station, clear the station's reference to it
		if (pickablePanel->ElectricPanelStationOn)
		{
			AElectricPanelStation* PanelStationOn = pickablePanel->ElectricPanelStationOn;
			// Clear station's stored panel pointer so station no longer references this panel
			PanelStationOn->ElectricPanelClass = nullptr;
			// Update splines so station state is consistent
			PanelStationOn->UpdateSplinesPanelStation();
			// Clear panel's back-reference
			pickablePanel->ElectricPanelStationOn = nullptr;
		}

		// Attach to mesh socket (or fallback) then fix scale/rotation
		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
		USkeletalMeshComponent* CharMesh = GetMesh();
		pickablePanel->AttachToComponent(CharMesh, AttachRules, TEXT("BigPanelSocket"));
		bPickTransition = true;
		bPick = true;
		HeldPanel = pickablePanel;
	}
}

void AWallEParent::OnPickDownStarted()
{
	if (!HeldPanel) return;

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

	// Get overlapping station actors safely and check array size before access
	TArray<AActor*> OverlapActors;
	PickBoxComp->GetOverlappingActors(OverlapActors, AElectricPanelStation::StaticClass());

	if (OverlapActors.Num() == 0)
	{
		return;
	}
	else
	{
	}

	for(AActor* Actor : OverlapActors)
	{
		if (!Actor) continue;

		if (AElectricPanelStation* Station = Cast<AElectricPanelStation>(Actor))
		{
			if(IsValid(Station->ElectricPanelClass))
			{
				continue;
			}
			if (!HeldPanel->IsCompatiblePanel(Station))
			{
				continue;
			}
			HeldPanel->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			bPickTransition = true;
			bPick = false;

			HeldPanel->SetActorLocationAndRotation(Station->GetActorLocation(), Station->GetActorRotation());
			Station->SetelectricPanelInformation(HeldPanel);
			Station->SetelectricStationPanelInformation(HeldPanel);
			Station->UpdateSplinesPanelStation();
			if (AElectricPanelPickable* HeldPanelPickable = Cast<AElectricPanelPickable>(HeldPanel))
			{
				// Pass the station so Blueprints can react with station context
				HeldPanelPickable->BP_OnSnapPanelEvent(Station);
			}
			HeldPanel = nullptr;
			break;
		}
	}
	//AElectricPanelStation* Station = Cast<AElectricPanelStation>(OverlapActors[0]);

	//if (Station && HeldPanel && !IsValid(Station->ElectricPanelClass))
	//{
	//	// Snap the held panel to the station's world transform (position + rotation)
	//	HeldPanel->SetActorLocationAndRotation(Station->GetActorLocation(), Station->GetActorRotation());
	//	Station -> SetelectricPanelInformation(HeldPanel);
	//	Station->SetelectricStationPanelInformation(HeldPanel);
	//	Station->UpdateSplinesPanelStation();
	//	if (AElectricPanelPickable* HeldPanelPickable = Cast<AElectricPanelPickable>(HeldPanel))
	//	{
	//		HeldPanelPickable->BP_OnSnapPanelEvent();
	//	}
	//}
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

void AWallEParent::OnLookInput(const FInputActionValue& Value)
{
    if (CameraToChange)
    {
        if (AWallECamera* WallECam = Cast<AWallECamera>(CameraToChange))
        {
            WallECam->AddOrbitInput(Value.Get<FVector2D>());
        }
    }
}

