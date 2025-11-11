// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PlayerController.h" 
#include "MainCamera.generated.h"

class UCameraComponent;
UCLASS()
class EA_ELECTRICPROYECT_API AMainCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainCamera();

	UPROPERTY()
	APlayerController* CachedPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool focusedCamera = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool activeCamera = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool moveCamera = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	AMainCamera* cameraToChange;
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void OnActivatedCamera();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void OnUnActivatedCamera();


	//UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Camera")
	//APawn* PawnToFollow = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Transition")
    bool bIsTransitioning = false;

	// Velocidad de movimiento (unidades por segundo)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Transition")
    float MoveSpeed = 300.0f;

    // Velocidad de rotaci車n (grados por segundo)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Transition")
    float RotSpeed = 90.0f;

    FVector StartLocation;
    FRotator StartRotation;
	UFUNCTION(BlueprintCallable, Category = "Camera|Transition")
    void StartCameraTransition();

public:
    // Duraci車n de la transici車n en segundos
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Transition")
    float TransitionDuration = 5.0f;

    // Tiempo acumulado de la transici車n
    float ElapsedTransitionTime = 0.0f;
};
