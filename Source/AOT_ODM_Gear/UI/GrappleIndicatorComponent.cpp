// Fill out your copyright notice in the Description page of Project Settings.


#include "AOT_ODM_Gear/UI/GrappleIndicatorComponent.h"
#include "GrappleIndicatorComponent.h"

UGrappleIndicatorComponent::UGrappleIndicatorComponent()
{
	WidgetComponentLocation = FVector::ZeroVector;
}

void UGrappleIndicatorComponent::OnComponentCreated()
{
	UE_LOG(LogTemp, Warning, TEXT("Widget created"));
}

void UGrappleIndicatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	SetWorldLocation(WidgetComponentLocation);
}
