// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BaseVRInteractable.h"
#include "MotionControllerComponent.h"
#include "Components/BoxComponent.h"
#include "InteractableHandComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRSTARTERPACK_API UInteractableHandComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractableHandComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/*Handles grip input for realistic grabbing*/
	UFUNCTION()
		void HandleRegularGrabInput(float AxisInput);

	/*Handles grip input for snap grabbing*/
	UFUNCTION()
		void HandleSnapGrabInput(float AxisInput, bool LeftHand);

	/*Grab anything in range of grab*/
	UFUNCTION()
		void AttemptGrab();

	/*Release anything held*/
	UFUNCTION()
		void AttemptRelease();

	UFUNCTION()
		void HandleAnimValues(float AxisValue);

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UMotionControllerComponent * MotionController;

	/*If the hand is static mesh*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UStaticMeshComponent * HandStaticMesh;

	/*If the hand is skeletal mesh*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		USkeletalMeshComponent * HandSkeletalMesh;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		USceneComponent * HandVisual;

	/*For detecting collisions and interactions*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UBoxComponent * HandOverlap;

	UPROPERTY(Category = Grabbing, EditAnywhere, BlueprintReadWrite)
		float CurrentGripAnimValue = 0.0f;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		UBaseVRInteractable * CachedTeleGrabObject = nullptr;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool CanTelegrab = false;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool CurrentlyGrabbed = false;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool ListeningForSnapGrab = true;

	/*How far down the grip has to be before firing off grab event*/
	UPROPERTY(Category = Grabbing, EditAnywhere)
		float GrabThreshold = 0.5f;

	/*For checking weather or not to fire grab event*/
	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool HandPastGrabThreshold = false;

	/*Storing current interactive object*/
	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		UBaseVRInteractable * CurrentHandInteraction;

	bool UseSkeletalMeshAsHands = true;
	bool LeftHand = false;
};
