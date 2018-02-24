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
		virtual void GrabOn(USceneComponent* Hand, USceneComponent * HandVisual, UInteractableHandComponent * HandLogic, bool TeleGrab, bool LeftHand) override;

	UFUNCTION()
		virtual void GrabOff(USceneComponent *Hand) override;

	UFUNCTION()
		void UpdateCurrentInteraction();

	UFUNCTION()
		void InitializeTrackingComponent();

	UPROPERTY(EditAnywhere)
		USceneComponent * TwistTrackingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool SnapHandMeshPosToInteraction = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (MakeEditWidget = true, EditCondition = "SnapHandMeshPosToInteraction"))
		FVector SnapHandVisualPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool SnapHandMeshRotToInteraction = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditCondition = "SnapHandMeshRotToInteraction"))
		FRotator SnapHandVisualRotation;

	/*Saving hand visuals transform for reattachment*/
	UPROPERTY()
		FTransform HandParentRelativeTransform;



	
	
};
