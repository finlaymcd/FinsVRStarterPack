// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseVRInteractable.h"
#include "PickupObjectComponent.h"
#include "SecondHandGrabComponent.generated.h"

/**
 * This component must be the child of a pickup object component to interact with it
 */
UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class VRSTARTERPACK_API USecondHandGrabComponent : public UBaseVRInteractable
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	UFUNCTION()
	virtual void GrabOn(USceneComponent* Hand, USceneComponent * HandVisual, UInteractableHandComponent * HandLogic, bool TeleGrab, bool LeftHand) override;

	UFUNCTION()
	virtual void GrabOff(USceneComponent *Hand) override;

	/*The parent pickup object that the other hand will be holding*/
	UPROPERTY()
	UPickupObjectComponent * PickupObjectParent = nullptr;

	/*Snap the visual of the hand to a predefined position while grabbing*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SnapHandMeshPosToInteraction = false;



	
	
};
