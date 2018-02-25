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

	UFUNCTION()
		void SetTelegrabTraceActive(USceneComponent * Hand, float Value);

	UFUNCTION()
		void SetTeleGrabType(EHandTeleGrabSystemEnum System, int InteractButtonForManual);

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

	/*Gets bound to interactables if they require it*/
	UPROPERTY()
		FHandInteractionNotificationDelegate TriggerDelegate;

	UPROPERTY()
		FHandInteractionNotificationDelegate FaceButtonOneDelegate;

	UPROPERTY()
		FHandInteractionNotificationDelegate FaceButtonTwoDelegate;

	UPROPERTY()
		FHandInteractionNotificationDelegate FaceButtonThreeDelegate;

	float TeleGrabMaxDistance = 300.0f;
	int TeleGrabButton = 1;
	bool UseSkeletalMeshAsHands = true;
	bool LeftHand = false;
	EHandTeleGrabSystemEnum TeleportGrabType = EHandTeleGrabSystemEnum::AutoTelegrab;
};
