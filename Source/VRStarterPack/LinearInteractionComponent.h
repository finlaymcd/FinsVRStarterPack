// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseVRInteractable.h"
#include "LinearInteractionComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VRSTARTERPACK_API ULinearInteractionComponent : public UBaseVRInteractable
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	//ULinearInteractionComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		virtual void GrabOn(USceneComponent* Hand, USceneComponent * HandVisual, bool TeleGrab, bool LeftHand) override;

	UFUNCTION()
		virtual void GrabOff(USceneComponent *Hand) override;

	UFUNCTION()
		void UpdateCurrentInteraction(); // On tick when grabbed
	
	/*Saved on begin play, rest location*/
	float SavedRelativeXLocation = 0.0f;
	float SavedRelativeYLocation = 0.0f;
	float SavedRelativeZLocation = 0.0f;

	/*If the interaction will move on the components local x axis*/
	UPROPERTY(Category = "Linear Interaction", EditAnywhere)
		bool InteractOnX = false;

	/*Minimum local X Value*/
	UPROPERTY(Category = "Linear Interaction", EditAnywhere, AdvancedDisplay, meta = (EditCondition = "InteractOnX"))
	float XMinDist = 0.0f;

	/*Maximum local X Value*/
	UPROPERTY(Category = "Linear Interaction", EditAnywhere, AdvancedDisplay, meta = (EditCondition = "InteractOnX"))
		float XMaxDist = 1.0f;

	/*If the interaction will move on the components local y axis*/
	UPROPERTY(Category = "Linear Interaction", EditAnywhere)
		bool InteractOnY = false;

	/*Minimum local Y Value*/
	UPROPERTY(Category = "Linear Interaction", EditAnywhere, AdvancedDisplay, meta = (EditCondition = "InteractOnY"))
		float YMinDist = 0.0f;

	/*Maximum local Y Value*/
	UPROPERTY(Category = "Linear Interaction", EditAnywhere, AdvancedDisplay, meta = (EditCondition = "InteractOnY"))
		float YMaxDist = 1.0f;

	/*If the interaction will move on the components local Z axis*/
	UPROPERTY(Category = "Linear Interaction", EditAnywhere)
		bool InteractOnZ = false;

	/*Minimum local Z Value*/
	UPROPERTY(Category = "Linear Interaction", EditAnywhere, AdvancedDisplay, meta = (EditCondition = "InteractOnZ"))
		float ZMinDist = 0.0f;

	/*Maximum local Z Value*/
	UPROPERTY(Category = "Linear Interaction", EditAnywhere, AdvancedDisplay, meta = (EditCondition = "InteractOnZ"))
		float ZMaxDist = 1.0f;

};
