// Fill out your copyright notice in the Description page of Project Settings.

#include "WallEParent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "MainCamera.h"
#include "ElectricPanel_RobotStation.h" // Asegúrate de incluir la cabecera
#include "ElectricPanelPickable.h"
#include "Components/PrimitiveComponent.h"
#include "Components/BoxComponent.h"

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
		}
		if (InteractStation)
		{
			EnhancedInput->BindAction(InteractStation, ETriggerEvent::Started, this, &AWallEParent::OnInteractStationStarted);
		}
		if (PickUp)
		{
			EnhancedInput->BindAction(PickUp, ETriggerEvent::Started, this, &AWallEParent::OnPickUpStarted);
		}
		if (PickDown)
		{
			EnhancedInput->BindAction(PickDown, ETriggerEvent::Started, this, &AWallEParent::OnPickDownStarted);
		}
	}
}

void AWallEParent::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

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
}

void AWallEParent::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

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

void AWallEParent::OnInteractStationStarted(const FInputActionValue& Value)
{
	if (RobotStationRef)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->Possess(RobotStationRef);

		}
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

	AElectricPanelPickable* Panel = Cast<AElectricPanelPickable>(OverlapActors[0]);
	if (!Panel) return;

	// Attach the panel to the PickBox component so it follows its position continually
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	Panel->AttachToComponent(PickBoxComp, AttachRules);

	// Optionally, disable physics or movement on the panel here if needed (not done by default)

	HeldPanel = Panel;
}

void AWallEParent::OnPickDownStarted()
{
	if (!HeldPanel) return;

	// Detach and keep world transform where it is, then clear reference
	HeldPanel->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	HeldPanel = nullptr;
}

