// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainCamera.generated.h"

class UCameraComponent;
UCLASS()
class EA_ELECTRICPROYECT_API AMainCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainCamera();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool defaultCamera = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool focusedCamera = false;

	//UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Camera")
	//APawn* PawnToFollow = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
