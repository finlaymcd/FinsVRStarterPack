// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BaseVRInteractable.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "MotionControllerComponent.h"
#include "Components/BoxComponent.h"
#include "InteractableHandComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHandInteractionNotificationDelegate, USceneComponent *, Hand, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHandDualAxisNotificationDelegate, USceneComponent *, Hand, float, XValue, float, YValue);

UENUM(BlueprintType)
enum class EHandTeleGrabSystemEnum : uint8
{
	ManualTelegrab 	UMETA(DisplayName = "Manual Teleport Grab"),
	AutoTelegrab 	UMETA(DisplayName = "Auto Teleport Grab"),
	NoTelegrab	UMETA(DisplayName = "No Teleport Grab")
};

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
	

	/*Receiving inputs from Pawn*/
	UFUNCTION()
		void ReceiveFaceButtonOne(bool ButtonDown);

	UFUNCTION()
		void ReceiveFaceButtonTwo(bool ButtonDown);

	UFUNCTION()
		void ReceiveFaceButtonThree(bool ButtonDown);

	UFUNCTION()
		void ReceiveTrigger(bool ButtonDown);

	UFUNCTION()
		void ReceiveDualAxisInput(float X, float Y);


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

	/*Saving out how much the hands are gripped to be used by anim bp*/
	UFUNCTION()
		void HandleAnimValues(float AxisValue);

	/*Handle Teleport grabbing system*/
	UFUNCTION()
		void HandleTeleGrab();

	/*Traces from an origin scene component for Interactable Objects*/
	UFUNCTION()
		UBaseVRInteractable* TeleGrabLineTrace(USceneComponent * TraceOrigin, bool DrawLine);

	/*If grabbing an interaction, read what controller input those interactions want, and bind them to the interaction delegates*/
	UFUNCTION()
		void SetupCurrentInteractionDelegates();

	/*Used by manual telegrabbing to turn it on and off as the player desires*/
	UFUNCTION()
		void SetTelegrabTraceActive(USceneComponent * Hand, float Value);

	/*Sets which type of telegrabbing to use, and which button to use if using manual tele grab*/
	UFUNCTION()
		void SetTeleGrabType(EHandTeleGrabSystemEnum System, int InteractButtonForManual);

	/*Motion controller component that this component is parented to*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UMotionControllerComponent * MotionController;

	/*If the hand is static mesh*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UStaticMeshComponent * HandStaticMesh;

	/*If the hand is skeletal mesh*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		USkeletalMeshComponent * HandSkeletalMesh;

	/*Generic reference to hand visual regardless of what is used*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		USceneComponent * HandVisual;

	/*For detecting collisions and interactions*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UBoxComponent * HandOverlap;

	/*Monitored for hand animation purposes*/
	UPROPERTY(Category = Grabbing, EditAnywhere, BlueprintReadWrite)
		float CurrentGripAnimValue = 0.0f;

	/*Saved by the tele grab system. If this is valid when the player grabs it will teleport to the players hand*/
	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		UBaseVRInteractable * CachedTeleGrabObject = nullptr;

	/*Should we be firing ray from hand for objects to telegrab*/
	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool CanTelegrab = false;

	/*is the hand currently holding something*/
	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool CurrentlyGrabbed = false;

	/*For the snap grab system, is the hand in an open position waiting to grab*/
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

	/*Gets bound to interactables if they require it*/
	UPROPERTY()
		FHandInteractionNotificationDelegate TriggerDelegate;

	UPROPERTY()
		FHandInteractionNotificationDelegate FaceButtonOneDelegate;

	UPROPERTY()
		FHandInteractionNotificationDelegate FaceButtonTwoDelegate;

	UPROPERTY()
		FHandInteractionNotificationDelegate FaceButtonThreeDelegate;

	UPROPERTY()
		FHandDualAxisNotificationDelegate DualAxisDelegate;

	/*These values are set in the BP editor on the Pawn actor. The Pawn then passes those values here*/
	float TeleGrabMaxDistance = 300.0f;
	int TeleGrabButton = 1;
	bool UseSkeletalMeshAsHands = true;
	bool LeftHand = false;
	EHandTeleGrabSystemEnum TeleportGrabType = EHandTeleGrabSystemEnum::AutoTelegrab;
};
