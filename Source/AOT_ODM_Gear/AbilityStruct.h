#pragma once

#include "Engine.h"

#include "AOT_ODM_Gear/UI/GrappleIndicatorComponent.h" 
#include "AbilityStruct.generated.h"


USTRUCT()
struct FGrappleTargetInfo
{
	GENERATED_BODY()

public:

	UPROPERTY()
	UGrappleIndicatorComponent* WidgetComp;

	UPROPERTY()
	FVector GrappleLocation;
};