// Fill out your copyright notice in the Description page of Project Settings.


#include "AOT_ODM_Gear/GameplayAbilities/GrappleAbility.h"
#include "AOT_ODM_Gear/AOT_ODM_GearCharacter.h"
#include "AOT_ODM_Gear/ODM_Gear.h"
#include "CableComponent.h"

void UGrappleAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	//UE_LOG(LogTemp, Warning, TEXT("Grapple ability activated"));

    if (AAOT_ODM_GearCharacter* PlayerCharacter = Cast<AAOT_ODM_GearCharacter>(GetAvatarActorFromActorInfo()))
    {
        if(PlayerCharacter->GetbCanGrapple())
        {
            //UE_LOG(LogTemp, Warning, TEXT("Valid Grapple Target, Grapple towards target"));

            // valid targets
            if (PlayerCharacter->GrappleTargetIndicators.Num() > 0)
            {
                TArray<AActor*> TargetKeys;
                PlayerCharacter->GrappleTargetIndicators.GetKeys(TargetKeys);

                // Get locations of cable components in world space
                FVector LeftCableLocation = PlayerCharacter->GetODMGearActor()->GetLeftCableComponent()->GetComponentLocation();
                FVector RightCableLocation = PlayerCharacter->GetODMGearActor()->GetRightCableComponent()->GetComponentLocation();

                // two valid grapple targets
                if(TargetKeys.Num() == 2)
                {
                    AActor* FirstGrappleTarget = TargetKeys[0];
                    AActor* SecondGrappleTarget = TargetKeys[1];

                    FVector FirstGrappleTargetLocation = FirstGrappleTarget->GetActorLocation();
                    FVector SecondGrappleTargetLocation = SecondGrappleTarget->GetActorLocation();


                    // Calculate distance from cables to grapple target
                    float DistanceToLeftCable = (FirstGrappleTargetLocation - LeftCableLocation).Length();
                    float DistanceToRightCable = (FirstGrappleTargetLocation - RightCableLocation).Length();

                    // If the left cable is closer to the first grapple target
                    if (DistanceToLeftCable < DistanceToRightCable)
                    {
                        PlayerCharacter->GetODMGearActor()->AttachGrappleCable(PlayerCharacter->GetODMGearActor()->GetLeftCableComponent(), FirstGrappleTarget, FirstGrappleTargetLocation);
                        PlayerCharacter->GetODMGearActor()->AttachGrappleCable(PlayerCharacter->GetODMGearActor()->GetRightCableComponent(), SecondGrappleTarget, SecondGrappleTargetLocation);
                    }

                    // if the right cable is closer to the first grapple target
                    else
                    {
                        PlayerCharacter->GetODMGearActor()->AttachGrappleCable(PlayerCharacter->GetODMGearActor()->GetRightCableComponent(), FirstGrappleTarget, FirstGrappleTargetLocation);
                        PlayerCharacter->GetODMGearActor()->AttachGrappleCable(PlayerCharacter->GetODMGearActor()->GetLeftCableComponent(), SecondGrappleTarget, SecondGrappleTargetLocation);
                    }

                    PlayerCharacter->SetbIsGrappling(true);
                }

                // @TODO if player is direcly looking at 1 object -> fire both cables at same object 
                
                // 1 valid grapple target 
                else if(TargetKeys.Num() == 1) 
                {
                    AActor* GrapleTarget = TargetKeys[0];

                    // Get the location of the widget UI component in world, this will be the grapple point
                    if (FGrappleTargetInfo* TargetInfo = PlayerCharacter->GrappleTargetIndicators.Find(TargetKeys[0]))
                    {
                        FVector GrappleLocation = TargetInfo->WidgetComp->GetComponentLocation();

                        //DrawDebugSphere(GetWorld(), GrappleLocation, 50.0f, 12, FColor::Orange, false, 5.0f);

                        FVector GrappleTargetLocation = GrappleLocation;

                        // Calculate distance from cables to grapple target
                        float DistanceToLeftCable = (GrappleTargetLocation - LeftCableLocation).Length();
                        float DistanceToRightCable = (GrappleTargetLocation - RightCableLocation).Length();



                        // If the left cable is closer to target
                        if (DistanceToLeftCable < DistanceToRightCable)
                        {
                            PlayerCharacter->GetODMGearActor()->AttachGrappleCable(PlayerCharacter->GetODMGearActor()->GetLeftCableComponent(), GrapleTarget, GrappleTargetLocation);
                            PlayerCharacter->GetODMGearActor()->bRightCableAttached = false;
                        }

                        else
                        {
                            PlayerCharacter->GetODMGearActor()->AttachGrappleCable(PlayerCharacter->GetODMGearActor()->GetRightCableComponent(), GrapleTarget, GrappleTargetLocation);
                            PlayerCharacter->GetODMGearActor()->bRightCableAttached = true;
                        }

                        PlayerCharacter->SetbIsGrappling(true);
                    }

               
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
