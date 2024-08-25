// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GrappleAbility.generated.h"


UCLASS()
class AOT_ODM_GEAR_API UGrappleAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	// Spawn an actor at the UI indicator location (the grapple point) so that the cable can attach to this actor 
	// Using set end location gives undefined behaviour when cable is not in player class
	UPROPERTY(EditDefaultsOnly, Category = "ODM Ability")
	TSubclassOf<AActor> FirstGrapplePointAttachActorClass;

	AActor* FirstGrapplePointAttachActor;

	void SpawnFirstGrapplePointAttachActor(FVector SpawnLocation);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
