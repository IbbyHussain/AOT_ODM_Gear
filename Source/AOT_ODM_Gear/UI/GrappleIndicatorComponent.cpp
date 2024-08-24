// Fill out your copyright notice in the Description page of Project Settings.


#include "AOT_ODM_Gear/UI/GrappleIndicatorComponent.h"
#include "GrappleIndicatorComponent.h"

UGrappleIndicatorComponent::UGrappleIndicatorComponent()
{
	WidgetComponentLocation = FVector::ZeroVector;
}

void UGrappleIndicatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//SetWorldLocation(WidgetComponentLocation);
}

