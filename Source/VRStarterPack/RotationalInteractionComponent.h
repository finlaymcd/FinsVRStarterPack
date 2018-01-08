// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseVRInteractable.h"
#include "Kismet/KismetMathLibrary.h"
#include "RotationalInteractionComponent.generated.h"


/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VRSTARTERPACK_API URotationalInteractionComponent : public UBaseVRInteractable
{
	GENERATED_BODY()
	
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	URotationalInteractionComponent();


	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void UpdateCurrentInteraction();

	UFUNCTION()
		void InitializeZRotGimbal();

	UPROPERTY(EditAnywhere)
		bool DualAxisInteraction = false;

	UPROPERTY(EditAnywhere)
		USceneComponent * ZRotGimbal;

	float SavedRelativePitch = 0.0f;
	float SavedRelativeRoll = 0.0f;
	float ZGimbalSavedRelativePitch = 0.0f;
	float ZGimbalSavedRelativeRoll = 0.0f;
	
};
