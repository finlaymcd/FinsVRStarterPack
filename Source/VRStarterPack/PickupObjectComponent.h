// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseVRInteractable.h"
#include "GameFramework/Actor.h"
#include "PickupObjectComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class VRSTARTERPACK_API UPickupObjectComponent : public UBaseVRInteractable
{
	GENERATED_BODY()
	
public:

	UFUNCTION()
		virtual void GrabOn(USceneComponent* Hand) override;

	UFUNCTION()
		virtual void GrabOff(USceneComponent *Hand) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool PhysicsObject = false;
};
