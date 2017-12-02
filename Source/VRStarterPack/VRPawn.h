// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MotionControllerComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "BaseVRInteractable.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "VRPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionNotificationDelegate, USceneComponent *, Hand, float, Value);




UCLASS()
class VRSTARTERPACK_API AVRPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVRPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/*Store Axis input for Movement to be applied on tick*/
	UFUNCTION()
		void CacheMovementInput_LX(float AxisInput);

	/*Store Axis input for Movement to be applied on tick*/
	UFUNCTION()
		void CacheMovementInput_LY(float AxisInput);

	/*Grabbing Input*/
	UFUNCTION()
		void InputLeftGrip(float AxisInput);

	/*Grabbing Input*/
	UFUNCTION()
		void InputRightGrip(float AxisInput);

	/*Main controller trigger input*/
	UFUNCTION()
		void LeftTriggerActionDown();

	/*Main controller trigger input*/
	UFUNCTION()
		void RightTriggerActionDown();

	/*vv     Face Buttons      vv*/
	UFUNCTION()
		void RightFaceButtonOneDown();

	UFUNCTION()
		void RightFaceButtonOneUp();

	UFUNCTION()
		void LeftFaceButtonOneDown();

	UFUNCTION()
		void LeftFaceButtonOneUp();

	UFUNCTION()
		void RightFaceButtonTwoDown();

	UFUNCTION()
		void RightFaceButtonTwoUp();

	UFUNCTION()
		void LeftFaceButtonTwoDown();

	UFUNCTION()
		void LeftFaceButtonTwoUp();

	/*Main controller trigger input*/
	UFUNCTION()
		void LeftTriggerActionUp();

	/*Main controller trigger input*/
	UFUNCTION()
		void RightTriggerActionUp();

	/*Handles grip input for realistic grabbing*/
	UFUNCTION()
		void HandleRegularGrabInput(float AxisInput, bool LeftHand);

	/*Handles grip input for snap grabbing*/
	UFUNCTION()
		void HandleSnapGrabInput(float AxisInput, bool LeftHand);

	/*Take cached axis input and apply to floating pawn movement*/
	UFUNCTION()
		void ApplyCachedMovement();

	/*Grab anything in range of grab*/
	UFUNCTION()
		void AttemptGrab(UBoxComponent * HandOverlap, UMotionControllerComponent * Hand);

	/*Release anything held*/
	UFUNCTION()
		void AttemptRelease(UBoxComponent * HandOverlap, UMotionControllerComponent * Hand);

	/*Blueprint Event coming off grab attempt*/
	UFUNCTION(BlueprintNativeEvent, Category = Grabbing)
		void NotifyAttemptGrab(USceneComponent * Hand, float Value);

	/*If grabbing an interaction, read what controller input those interactions want, and bind them to the interaction delegates*/
	UFUNCTION()
		void SetupCurrentInteractionDelegates(bool LeftHand);

	/*Root capsule component*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UCapsuleComponent * Root;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UMotionControllerComponent * RMotionController;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UMotionControllerComponent * LMotionController;

	/*If the hand is static mesh*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UStaticMeshComponent * LHandStaticMesh;

	/*If the hand is static mesh*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UStaticMeshComponent * RHandStaticMesh;

	/*If the hand is skeletal mesh*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		USkeletalMeshComponent * RHandSkeletalMesh;

	/*If the hand is skeletal mesh*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		USkeletalMeshComponent * LHandSkeletalMesh;

	/*For detecting collisions and interactions*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UBoxComponent * RHandOverlap;

	/*For detecting collisions and interactions*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UBoxComponent * LHandOverlap;

	/*For forcing playing camera location*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		USceneComponent * TrackingOrigin;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UCameraComponent * PlayerCamera;

	/*Handles movement*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UFloatingPawnMovement * FloatingPawnMovement;

	/*Speed to apply to floating pawn movement*/
	UPROPERTY(Category = Movement, EditAnywhere)
		float MoveSpeed = 1.0f;

	/*Speed for turning if using smooth turning*/
	UPROPERTY(Category = Turning, EditAnywhere)
		float TurnSpeed = 1.0f;

	/*How far down the grip has to be before firing off grab event*/
	UPROPERTY(Category = Grabbing, EditAnywhere)
		float GrabThreshold = 0.5f;

	/*If snap grab is active, player doesnt have to hold down grip button*/
	UPROPERTY(Category = Grabbing, EditAnywhere, BlueprintReadWrite)
		bool SnapGrab = false;

	/*For checking weather or not to fire grab event*/
	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool RightHandPastGrabThreshold = false;

	/*For checking weather or not to fire grab event*/
	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool LeftHandPastGrabThreshold = false;

	/*For firing delegates off end of grab event*/
	UPROPERTY(BlueprintAssignable, Category = Grabbing)
		FInteractionNotificationDelegate GrabDelegate;

	/*Gets bound to interactables if they require it*/
	UPROPERTY()
		FInteractionNotificationDelegate RightTriggerDelegate;

	/*Gets bound to interactables if they require it*/
	UPROPERTY()
		FInteractionNotificationDelegate LeftTriggerDelegate;

	UPROPERTY()
		FInteractionNotificationDelegate LeftFaceButtonOneDelegate;

	UPROPERTY()
		FInteractionNotificationDelegate LeftFaceButtonTwoDelegate;

	UPROPERTY()
		FInteractionNotificationDelegate RightFaceButtonOneDelegate;

	UPROPERTY()
		FInteractionNotificationDelegate RightFaceButtonTwoDelegate;

	/*Storing current interactive object*/
	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		UBaseVRInteractable * CurrentLeftHandInteraction;

	/*Storing current interactive object*/
	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		UBaseVRInteractable * CurrentRightHandInteraction;

	FVector CurrentMovementInput = FVector(0.0f, 0.0f, 0.0f);
};
