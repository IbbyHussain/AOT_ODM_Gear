// Fill out your copyright notice in the Description page of Project Settings.


#include "AOT_ODM_Gear/GameplayAbilities/GrappleAbility_FindValidTarget.h"
#include "DrawDebugHelpers.h"
#include "AOT_ODM_Gear/AOT_ODM_GearCharacter.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AOT_ODM_Gear/UI/GrappleIndicatorComponent.h"
#include "AOT_ODM_Gear/ODM_Gear.h"
#include "AOT_ODM_Gear/AbilityStruct.h"


UGrappleAbility_FindValidTarget::UGrappleAbility_FindValidTarget()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    MaxGrappleDistance = 5000.0f;

    MaxGrappleAngle = 0.5f;
}

void UGrappleAbility_FindValidTarget::PerformSphereTrace()
{
    if (PlayerCharacter && PlayerController && !PlayerCharacter->GetbIsGrappling())
    {
        TArray<AActor*> AllTargets;

        TArray<AActor*> AllGrappleTargets;

        AllTargets = PlayerCharacter->GetGrappleBoxCompOverlappingActors();

        for(auto Target : AllTargets)
        {
            if(Target && Target->ActorHasTag(FName("GrappleTarget")))
            {
                AllGrappleTargets.Add(Target);
            }
        }
   
        FVector CameraLocation;
        FRotator CameraRotation;
        PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

        FVector CameraForwardVector = CameraRotation.Vector();
        
        /* Sort grapple targets by distance to the camera, this allows for a max of two grapple targets (similar to ODM) */
        AllGrappleTargets.Sort([&](AActor& A, AActor& B)
            {
                FVector GrappleLocationA = GetClosestPointOnActorCollision(&A, CameraLocation);
                FVector GrappleLocationB = GetClosestPointOnActorCollision(&B, CameraLocation);

                float DistanceA = FVector::Dist(CameraLocation, GrappleLocationA);
                float DistanceB = FVector::Dist(CameraLocation, GrappleLocationB);
                return DistanceA < DistanceB;
            });

        // Allow for only 2 grapple points
        if (AllGrappleTargets.Num() > 2)
        {
            AllGrappleTargets.SetNum(2);
        }

        /* Spawn UI indicator on valid grapple target */
        TMap<AActor*, FGrappleTargetInfo> NewGrappleTargetIndicators;

        for (AActor* Target : AllGrappleTargets)
        {
            FVector CameraTraceEnd = CameraLocation + (CameraForwardVector * MaxGrappleDistance);

            FHitResult CameraTraceHitResult;
            FCollisionQueryParams CameraTraceQueryParams;

            bool bHit = GetWorld()->LineTraceSingleByChannel(CameraTraceHitResult, CameraLocation, CameraTraceEnd, ECC_Visibility, CameraTraceQueryParams);
            DrawDebugLine(GetWorld(), CameraLocation, CameraTraceEnd, FColor::Red, false, 1.0f, 0.0f, 1.0f);

            // If the camera trace hits an object
            FVector GrappleLocation;

            if(bHit)
            {
                // If the hit object is a grapple target, grapple point will be on the target 
                if(CameraTraceHitResult.GetActor() == Target)
                {
                    GrappleLocation = CameraTraceHitResult.ImpactPoint;
                    PlayerCharacter->SetbIsLookingAtTarget(true);
                }

                // If the hit object is NOT a grapple target, then use the impact point (of trace) to get the closest point to the grapple target and this will be be grapple point
                else
                {
                    GrappleLocation = GetClosestPointOnActorCollision(Target, CameraTraceHitResult.ImpactPoint);
                    PlayerCharacter->SetbIsLookingAtTarget(false);
                }
            }

            // If the camera trace did not hit anything then use endpoint of trace to get the closest point to the grapple target and this will be be grapple point
            else
            {
                GrappleLocation = GetClosestPointOnActorCollision(Target, CameraTraceEnd);
                PlayerCharacter->SetbIsLookingAtTarget(false);
            }

            SpawnUIIndicator(Target, GrappleLocation, NewGrappleTargetIndicators);
            DrawDebugSphere(GetWorld(), GrappleLocation, 20.0f, 12, FColor::Red, false, 1.0f);
        }

        // Destroy UI indicators for targets that are no longer valid
        for (auto& IndicatorPair : PlayerCharacter->GrappleTargetIndicators)
        {
            if (IndicatorPair.Value.WidgetComp)
            {
                IndicatorPair.Value.WidgetComp->DestroyComponent();
            }
        }

        // Update the map with the current valid targets and their indicators
        PlayerCharacter->GrappleTargetIndicators = NewGrappleTargetIndicators;

        // Allow the player to grapple if there is at least 1 valid grapple target 
        PlayerCharacter->GrappleTargetIndicators.Num() > 0 ? PlayerCharacter->SetbCanGrapple(true) : PlayerCharacter->SetbCanGrapple(false);
    }
}

void UGrappleAbility_FindValidTarget::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (ActorInfo && ActorInfo->AvatarActor.IsValid())
    {
        // Validate pointers to player and controller 
        PlayerCharacter = Cast<AAOT_ODM_GearCharacter>(GetAvatarActorFromActorInfo());

        if(PlayerCharacter)
        {
            PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
        }

        PerformSphereTrace();

        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
    }
}

FVector UGrappleAbility_FindValidTarget::GetClosestPointOnActorCollision(AActor* Target, FVector StartPoint)
{
    if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Target->GetRootComponent()))
    {
        FVector ClosestPoint;

        PrimitiveComponent->GetClosestPointOnCollision(StartPoint, ClosestPoint);

        //DrawDebugSphere(GetWorld(), ClosestPoint, 20.0f, 12, FColor::Green, false, 1.0f);

        return ClosestPoint;
    }

    return StartPoint;
}

void UGrappleAbility_FindValidTarget::SpawnUIIndicator(AActor* Target, FVector SpawnLocation, TMap<AActor*, FGrappleTargetInfo>& NewGrappleTargetIndicators)
{
    /* Spawn UI indicator on valid grapple target */
    if (PlayerCharacter->GrappleTargetIndicators.Contains(Target))
    {
        // If the indicator already exists, just keep it

        FGrappleTargetInfo& GrappleTargetInfo = PlayerCharacter->GrappleTargetIndicators[Target];
        if (GrappleTargetInfo.WidgetComp)
        {
            GrappleTargetInfo.GrappleLocation = SpawnLocation; //useless
            GrappleTargetInfo.WidgetComp->SetWorldLocation(GrappleTargetInfo.GrappleLocation);

            NewGrappleTargetIndicators.Add(Target, PlayerCharacter->GrappleTargetIndicators[Target]);
        }

        PlayerCharacter->GrappleTargetIndicators.Remove(Target);
    }

    else
    {
        // Create a new widget component for the grapple point
        UGrappleIndicatorComponent* WidgetComp = NewObject<UGrappleIndicatorComponent>(Target);

        if (WidgetComp && GrapplePointWidget)
        {
            WidgetComp->SetupAttachment(Target->GetRootComponent()); // Attach to the target actor
            WidgetComp->SetWidgetClass(GrapplePointWidget);
            WidgetComp->SetRelativeLocation(FVector::ZeroVector);

            FGrappleTargetInfo GrappleInfo;
            GrappleInfo.WidgetComp = WidgetComp;
            GrappleInfo.GrappleLocation = SpawnLocation;

            WidgetComp->SetWorldLocation(GrappleInfo.GrappleLocation);

            WidgetComp->SetWidgetSpace(EWidgetSpace::Screen); // Use screen space for 2D UI
            WidgetComp->RegisterComponent();

            NewGrappleTargetIndicators.Add(Target, GrappleInfo);
        }
    }
}



