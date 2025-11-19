// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ElectricSplineOpneCloseSubsystem.generated.h"

// Dynamic multicast delegate: one bool parameter indicating open(true)/close(false)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSplineOpenClose, bool, bIsOpen);

/**
 * Subsystem that exposes a multicast event to C++ and Blueprints.
 */
UCLASS()
class UElectricSplineOpneCloseSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Blueprint can bind to this (Assign / Add Event)
    UPROPERTY(BlueprintAssignable, Category = "ElectricSpline")
    FOnSplineOpenClose OnSplineOpenClose;

    // Broadcast from C++ (or Blueprint can call this function)
    UFUNCTION(BlueprintCallable, Category = "ElectricSpline")
    void BroadcastSplineOpenClose(bool bOpen);

    // Optional helper call from C++
    UFUNCTION(BlueprintCallable, Category = "ElectricSpline")
    void OpenSpline() { BroadcastSplineOpenClose(true); }

    UFUNCTION(BlueprintCallable, Category = "ElectricSpline")
    void CloseSpline() { BroadcastSplineOpenClose(false); }
};
