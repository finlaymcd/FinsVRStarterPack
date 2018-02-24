// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseVRInteractable.h"
#include "Kismet/KismetMathLibrary.h"
#include "RotationalInteractionComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInteractionValueUpdate, USceneComponent *, Hand, float, XValue, float, YValue);

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
		virtual void GrabOn(USceneComponent* Hand, USceneComponent * HandVisual, UInteractableHandComponent * HandLogic, bool TeleGrab, bool LeftHand) override;

	UFUNCTION()
		virtual void GrabOff(USceneComponent *Hand) override;

	UFUNCTION()
		void UpdateCurrentInteraction();

	//UFUNCTION()
	//	void UpdateHandVisuals();

	UFUNCTION()
		float CalculateRotPercentage(float min, float max, float current);

	UFUNCTION()
		void InitializeZRotGimbal();

	UFUNCTION(BlueprintNativeEvent, Category = RotationInteraction)
		void BlueprintUpdateRotValues(USceneComponent * RotComponent, float XValue, float YValue);

	UFUNCTION()
		void ReturnToStartRotation();

	UPROPERTY(EditAnywhere, Category = RotationInteraction)
		bool DualAxisInteraction = false;

	UPROPERTY(EditAnywhere, Category = RotationInteraction)
		bool SnapHandMeshToInteraction = false;

	UPROPERTY()
		FTransform HandParentRelativeTransform;

	UPROPERTY(EditAnywhere, Category = RotationInteraction, Meta = (MakeEditWidget = true))
		FVector HandSnapLocation;

	UPROPERTY(EditAnywhere, Category = RotationInteraction)
		FRotator HandSnapRotation;

	UPROPERTY(EditAnywhere, Category = RotationInteraction)
		USceneComponent * ZRotGimbal;

	float SavedRelativePitch = 0.0f;
	float SavedRelativeRoll = 0.0f;
	float ZGimbalSavedRelativePitch = 0.0f;
	float ZGimbalSavedRelativeRoll = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = RotationInteraction)
		float CurrentXAngle = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = RotationInteraction)
		float CurrentZAngle = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = RotationInteraction)
		float XValue = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = RotationInteraction)
		float ZValue = 0.0f;

	UPROPERTY(EditAnywhere, Category = RotationInteraction)
		float UpperAngleLimit = 45.0f;

	UPROPERTY(EditAnywhere, Category = RotationInteraction)
		float LowerAngleLimit = -45.0f;

	UPROPERTY(EditAnywhere, Category = RotationInteraction)
		float ZUpperAngleLimit = 45.0f;

	UPROPERTY(EditAnywhere, Category = RotationInteraction)
		float ZLowerAngleLimit = -45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationInteraction)
		bool ReturnToStartOnRelease = true;

	UPROPERTY(BlueprintReadWrite, Category = RotationInteraction)
		float XStartValue = 0.0f;

	UPROPERTY(BlueprintReadWrite, Category = RotationInteraction)
		float ZStartValue = 0.0f;

	UPROPERTY(BlueprintAssignable, Category = RotationInteraction)
		FInteractionValueUpdate BlueprintRotationInteractionUpdate;

	UPROPERTY()
		FInteractionValueUpdate InteractionUpdate;
	


};
