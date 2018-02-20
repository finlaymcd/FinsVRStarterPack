// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseVRInteractable.h"
#include "Kismet/KismetMathLibrary.h"
#include "TwistInteractionComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VRSTARTERPACK_API UTwistInteractionComponent : public UBaseVRInteractable
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		virtual void GrabOn(USceneComponent* Hand, USceneComponent * HandVisual, bool TeleGrab, bool LeftHand) override;

	UFUNCTION()
		virtual void GrabOff(USceneComponent *Hand) override;

	UFUNCTION()
		void UpdateCurrentInteraction();

	UFUNCTION()
		void InitializeTrackingComponent();

	UPROPERTY(EditAnywhere)
		USceneComponent * TwistTrackingComponent;

	
	
};
