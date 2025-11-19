// Fill out your copyright notice in the Description page of Project Settings.


#include "EventActors/ElectricSplineOpneCloseSubsystem.h"

void UElectricSplineOpneCloseSubsystem::BroadcastSplineOpenClose(bool bOpen)
{
    // Safety check not strictly required for multicast delegates, but kept for clarity
    OnSplineOpenClose.Broadcast(bOpen);
}