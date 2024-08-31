// Fill out your copyright notice in the Description page of Project Settings.


#include "AOT_ODM_Gear/GameplayAbilities/GrappleAbility.h"
#include "AOT_ODM_Gear/AOT_ODM_GearCharacter.h"
#include "AOT_ODM_Gear/ODM_Gear.h"
#include "CableComponent.h"

AActor* UGrappleAbility::SpawnFirstGrapplePointAttachActor(FVector SpawnLocation)
{
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* GrappleActor = GetWorld()->SpawnActor<AActor>(FirstGrapplePointAttachActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

    return GrappleActor;
}

//@TODO make it so that the length width and height can be changed using variables 
void UGrappleAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	//UE_LOG(LogTemp, Warning, TEXT("Grapple ability activated"));

    if (AAOT_ODM_GearCharacter* PlayerCharacter = Cast<AAOT_ODM_GearCharacter>(GetAvatarActorFromActorInfo()))
    {
        if(PlayerCharacter->GetbCanGrapple())
        {
            // valid targets
            if (PlayerCharacter->GrappleTargetIndicators.Num() > 0)
            {
                TArray<AActor*> TargetKeys;
                PlayerCharacter->GrappleTargetIndicators.GetKeys(TargetKeys);

                // Get locations of cable components in world space
                FVector LeftCableLocation = PlayerCharacter->GetODMGearActor()->GetLeftCableComponent()->GetComponentLocation();
                FVector RightCableLocation = PlayerCharacter->GetODMGearActor()->GetRightCableComponent()->GetComponentLocation();

                /* Grappling with two Targets */
                if(TargetKeys.Num() == 2)
                {
                    AActor* FirstGrappleTarget = TargetKeys[0];
                    AActor* SecondGrappleTarget = TargetKeys[1];
                    
                    FGrappleTargetInfo& FirstTargetInfo = PlayerCharacter->GrappleTargetIndicators[FirstGrappleTarget];
                    FGrappleTargetInfo& SecondTargetInfo = PlayerCharacter->GrappleTargetIndicators[SecondGrappleTarget];

                    // Get the UI indciator world location (Grapple point)
                    FVector FirstGrapplePointLocation = FirstTargetInfo.GrappleLocation;
                    FVector SecondGrapplePointLocation = SecondTargetInfo.GrappleLocation;// Can just get the world location of the UI comp

                    AActor* FirstGrapplePointAttachActor = SpawnFirstGrapplePointAttachActor(FirstGrapplePointLocation);
                    AActor* SecondGrapplePointAttachActor = SpawnFirstGrapplePointAttachActor(SecondGrapplePointLocation);

                    // Calculate distance from cables to grapple target
                    float DistanceToLeftCable = (FirstGrapplePointLocation - LeftCableLocation).Length();
                    float DistanceToRightCable = (FirstGrapplePointLocation - RightCableLocation).Length();

                    // If the left cable is closer to the first grapple target
                    if (DistanceToLeftCable < DistanceToRightCable)
                    {
                        PlayerCharacter->GetODMGearActor()->AttachGrappleCable(PlayerCharacter->GetODMGearActor()->GetLeftCableComponent(), FirstGrapplePointAttachActor);
                        PlayerCharacter->GetODMGearActor()->AttachGrappleCable(PlayerCharacter->GetODMGearActor()->GetRightCableComponent(), SecondGrapplePointAttachActor);
                    }

                    // if the right cable is closer to the first grapple target
                    else
                    {
                        PlayerCharacter->GetODMGearActor()->AttachGrappleCable(PlayerCharacter->GetODMGearActor()->GetRightCableComponent(), FirstGrapplePointAttachActor);
                        PlayerCharacter->GetODMGearActor()->AttachGrappleCable(PlayerCharacter->GetODMGearActor()->GetLeftCableComponent(), SecondGrapplePointAttachActor);
                    }

                    PlayerCharacter->SetbIsGrappling(true);
                }

                /* Grappling with one Target */
                else if(TargetKeys.Num() == 1) 
                {
                    AActor* GrapleTarget = TargetKeys[0];

                    // Get the location of the widget UI component in world, this will be the grapple point
                    FGrappleTargetInfo& TargetInfo = PlayerCharacter->GrappleTargetIndicators[GrapleTarget];
                    
					// Get the UI indciator world location (Grapple point)
					FVector GrapplePointLocation = TargetInfo.GrappleLocation; // Can just get the world location of the UI comp

                    AActor* FirstGrapplePointAttachActor = SpawnFirstGrapplePointAttachActor(GrapplePointLocation);

                    // If the player is direcly looking at a target then shoot two cables at grapple location
                    if(PlayerCharacter->GetbIsLookingAtTarget())
                    {
                        PlayerCharacter->GetODMGearActor()->AttachGrappleCable(PlayerCharacter->GetODMGearActor()->GetRightCableComponent(), FirstGrapplePointAttachActor);
                        PlayerCharacter->GetODMGearActor()->AttachGrappleCable(PlayerCharacter->GetODMGearActor()->GetLeftCableComponent(), FirstGrapplePointAttachActor);
                    }

                    // If the player is not direcly looking at target, shoot 1 cable at target
                    else
                    {
                        // Calculate distance from cables to grapple target
                        float DistanceToLeftCable = (GrapplePointLocation - LeftCableLocation).Length();
                        float DistanceToRightCable = (GrapplePointLocation - RightCableLocation).Length();

                        // If the left cable is closer to target, use the left cable
                        if (DistanceToLeftCable < DistanceToRightCable)
                        {
                            PlayerCharacter->GetODMGearActor()->AttachGrappleCable(PlayerCharacter->GetODMGearActor()->GetLeftCableComponent(), FirstGrapplePointAttachActor);
                        }

                        else
                        {
                            PlayerCharacter->GetODMGearActor()->AttachGrappleCable(PlayerCharacter->GetODMGearActor()->GetRightCableComponent(), FirstGrapplePointAttachActor);
                        }
                    }

					PlayerCharacter->SetbIsGrappling(true);
                }

                // No valid grapple targets 
                else
                {
                    return;
                }
            }
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
