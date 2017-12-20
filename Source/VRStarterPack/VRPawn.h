// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MotionControllerComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Engine.h"
#include "TimerManager.h"
#include "BaseVRInteractable.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "VRPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionNotificationDelegate, USceneComponent *, Hand, float, Value);


UENUM(BlueprintType)
enum class ERotationSystemEnum : uint8
{
	SmoothTurn 	UMETA(DisplayName = "Smooth Turn"),
	SnapTurn 	UMETA(DisplayName = "Snap Turn"),
	RoomScale	UMETA(DisplayName = "Room Scale (Disabled)")
};

UENUM(BlueprintType)
enum class EMovementSystemEnum : uint8
{
	FreeLoco 	UMETA(DisplayName = "Free Locomotion"),
	Teleport 	UMETA(DisplayName = "Teleporting"),
	RoomScale	UMETA(DisplayName = "Room Scale (Disabled)")
};

UENUM(BlueprintType)
enum class ETeleGrabSystemEnum : uint8
{
	ManualTelegrab 	UMETA(DisplayName = "Manual Teleport Grab"),
	AutoTelegrab 	UMETA(DisplayName = "Auto Teleport Grab"),
	NoTelegrab	UMETA(DisplayName = "No Teleport Grab")
};



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

	UFUNCTION()
		void InitializePawnControls();

	/*Store Axis input for Movement to be applied on tick*/
	UFUNCTION()
		void CacheMovementInput_LX(float AxisInput);

	/*Store Axis input for Movement to be applied on tick*/
	UFUNCTION()
		void CacheMovementInput_LY(float AxisInput);

	/*Store Axis input for Movement to be applied on tick*/
	UFUNCTION()
		void CacheMovementInput_RX(float AxisInput);

	UFUNCTION()
		void CacheMovementInput_RY(float AxisInput);

	UFUNCTION()
		void HandlePlayerRotation(float AxisInput);
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

	UFUNCTION()
		void DrawTeleportArc();

	UFUNCTION()
		void TeleportPlayer();

	/*Grab anything in range of grab*/
	UFUNCTION()
		void AttemptGrab(UBoxComponent * HandOverlap, UMotionControllerComponent * Hand, UBaseVRInteractable * CachedTelegrabObject);

	/*Release anything held*/
	UFUNCTION()
		void AttemptRelease(UBoxComponent * HandOverlap, UMotionControllerComponent * Hand);

	/*Handle Teleport grabbing system*/
	UFUNCTION()
		void HandleTeleGrab();

	/*Traces from an origin scene component for Interactable Objects*/
	UFUNCTION()
		UBaseVRInteractable* TeleGrabLineTrace(USceneComponent * TraceOrigin, bool DrawLine);

	UFUNCTION()
		void SetTelegrabTraceActive(USceneComponent * Hand, float Value);

	/*Blueprint Event coming off grab attempt*/
	UFUNCTION(BlueprintNativeEvent, Category = Grabbing)
		void NotifyAttemptGrab(USceneComponent * Hand, float Value);

	/*If grabbing an interaction, read what controller input those interactions want, and bind them to the interaction delegates*/
	UFUNCTION()
		void SetupCurrentInteractionDelegates(bool LeftHand);

	/*Resets the players ability to snap turn*/
	UFUNCTION()
		void ResetSnapTurn();

	UFUNCTION()
		void HandleHandAnimValues(bool LeftHand, float AxisValue);

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

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UStaticMeshComponent * TeleportIndicator;

	/*Handles movement*/
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UFloatingPawnMovement * FloatingPawnMovement;

	/*Speed to apply to floating pawn movement*/
	UPROPERTY(Category = Movement, EditAnywhere)
		float MoveSpeed = 1.0f;

	/*If true, linear movement will be taken from the left motion controller. If not, the right*/
	UPROPERTY(Category = Movement, EditAnywhere)
		bool MovementOnLeftHand = true;

	UPROPERTY(Category = Movement, EditAnywhere)
		bool RotationOnLeftHand = false;

	UPROPERTY(Category = Movement, EditAnywhere)
		float TeleportDistance = 300.0f;

	UPROPERTY(Category = Movement, BlueprintReadOnly)
		bool TeleportButtonDown = false;

	UPROPERTY(Category = Movement, BlueprintReadOnly)
		bool DrawingTeleportArc = false;

	FVector CachedTeleportLocation = FVector::ZeroVector;
	
	bool TeleportLocationIsValid = false;

	/*How the player will rotate in the world*/
	UPROPERTY(Category = Movement, EditAnywhere)
		ERotationSystemEnum RotationType = ERotationSystemEnum::SnapTurn;

	/*How the player will rotate in the world*/
	UPROPERTY(Category = Movement, EditAnywhere)
		EMovementSystemEnum MovementType = EMovementSystemEnum::FreeLoco;

	/*Speed for turning if using smooth turning*/
	UPROPERTY(Category = Movement, EditAnywhere)
		float SmoothTurnSpeed = 1.0f;

	/*How far a single snap turn turns a player*/
	UPROPERTY(Category = Movement, EditAnywhere)
		float SnapTurnAngle = 20.0f;

	/*Time to wait between a snap turn and allowing the player to do the next snap turn*/
	UPROPERTY(Category = Movement, EditAnywhere)
		float SnapTurnDelay = 0.4f;

	/*Times the delay between snap turns*/
	UPROPERTY()
	FTimerHandle SnapTurnDelayTimerHandle;

	UPROPERTY(Category = Movement, BlueprintReadWrite)
		bool CanSnapTurn = true;

	/*How far down the grip has to be before firing off grab event*/
	UPROPERTY(Category = Grabbing, EditAnywhere)
		float GrabThreshold = 0.5f;

	UPROPERTY(Category = Grabbing, BlueprintReadWrite)
		float CurrentGripAnimValue_L = 0.0f;

	UPROPERTY(Category = Grabbing, BlueprintReadWrite)
		float CurrentGripAnimValue_R = 0.0f;

	/*If snap grab is active, player doesnt have to hold down grip button*/
	UPROPERTY(Category = Grabbing, EditAnywhere, BlueprintReadWrite)
		bool SnapGrab = false;

	UPROPERTY(Category = Grabbing, EditAnywhere, BlueprintReadWrite)
		ETeleGrabSystemEnum TeleportGrabType = ETeleGrabSystemEnum::AutoTelegrab;

	UPROPERTY(Category = Grabbing, EditAnywhere, BlueprintReadWrite)
		float TeleGrabMaxDistance = 300.0f;


	/*Is much more forgiving for players who are not accurately grabbing at objects, but is a more expensive operation on the game thread*/
	UPROPERTY(Category = Grabbing, EditAnywhere, BlueprintReadWrite)
		bool BoxTraceForTelegrab = false;

	UPROPERTY(Category = Grabbing, EditAnywhere, BlueprintReadWrite)
		bool UseSkeletalMeshAsHands = true;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		UBaseVRInteractable * CachedTeleGrabObjectLeft = nullptr;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		UBaseVRInteractable * CachedTeleGrabObjectRight = nullptr;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool CanTelegrabLeft = false;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool CanTelegrabRight = false;

	UPROPERTY(Category = Grabbing, EditAnywhere, BlueprintReadWrite)
		EInteractButtonEnum ManualTelegrabButton = EInteractButtonEnum::ButtonOne;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool LeftCurrentlyGrabbed = false;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool RightCurrentlyGrabbed = false;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool LeftListeningForSnapGrab = true;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool RightListeningForSnapGrab = true;

	/*For checking weather or not to fire grab event*/
	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool RightHandPastGrabThreshold = false;

	/*For checking weather or not to fire grab event*/
	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool LeftHandPastGrabThreshold = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics)
		float PhysicsThrowStrength = 1.0f;

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
