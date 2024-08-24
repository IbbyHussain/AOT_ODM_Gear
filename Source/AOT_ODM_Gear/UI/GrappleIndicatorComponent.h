// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GrappleIndicatorComponent.generated.h"


UCLASS()
class AOT_ODM_GEAR_API UGrappleIndicatorComponent : public UWidgetComponent
{
	GENERATED_BODY()

private:

	UGrappleIndicatorComponent();

	FVector WidgetComponentLocation;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:

	void SetWidgetComponentLocation(FVector NewLocation) { WidgetComponentLocation = NewLocation; }
};
