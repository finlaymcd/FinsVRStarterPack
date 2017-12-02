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


	UFUNCTION()
		void CacheMovementInput_LX(float AxisInput);

	UFUNCTION()
		void CacheMovementInput_LY(float AxisInput);


	UFUNCTION()
		void InputLeftGrip(float AxisInput);

	UFUNCTION()
		void InputRightGrip(float AxisInput);

	UFUNCTION()
		void LeftTriggerActionDown();

	UFUNCTION()
		void RightTriggerActionDown();

	UFUNCTION()
		void LeftTriggerActionUp();

	UFUNCTION()
		void RightTriggerActionUp();

	UFUNCTION()
		void HandleRegularGrabInput(float AxisInput, bool LeftHand);

	UFUNCTION()
		void HandleSnapGrabInput(float AxisInput, bool LeftHand);

	UFUNCTION()
		void ApplyCachedMovement();

	UFUNCTION()
		void AttemptGrab(UBoxComponent * HandOverlap, UMotionControllerComponent * Hand);

	UFUNCTION()
		void AttemptRelease(UBoxComponent * HandOverlap, UMotionControllerComponent * Hand);

	UFUNCTION(BlueprintNativeEvent, Category = Grabbing)
		void NotifyAttemptGrab(USceneComponent * Hand, float Value);

	UFUNCTION()
		void SetupCurrentInteractionDelegates(bool LeftHand);


	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UCapsuleComponent * Root;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UMotionControllerComponent * RMotionController;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UMotionControllerComponent * LMotionController;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UStaticMeshComponent * LHandStaticMesh;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UStaticMeshComponent * RHandStaticMesh;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		USkeletalMeshComponent * RHandSkeletalMesh;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		USkeletalMeshComponent * LHandSkeletalMesh;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UBoxComponent * RHandOverlap;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UBoxComponent * LHandOverlap;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		USceneComponent * TrackingOrigin;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UCameraComponent * PlayerCamera;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UFloatingPawnMovement * FloatingPawnMovement;

	UPROPERTY(Category = Movement, EditAnywhere)
		float MoveSpeed = 1.0f;

	UPROPERTY(Category = Turning, EditAnywhere)
		float TurnSpeed = 1.0f;

	UPROPERTY(Category = Grabbing, EditAnywhere)
		float GrabThreshold = 0.5f;

	UPROPERTY(Category = Grabbing, EditAnywhere, BlueprintReadWrite)
		bool SnapGrab = false;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool RightHandPastGrabThreshold = false;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		bool LeftHandPastGrabThreshold = false;

	UPROPERTY(BlueprintAssignable, Category = Grabbing)
		FInteractionNotificationDelegate GrabDelegate;

	UPROPERTY()
		FInteractionNotificationDelegate RightTriggerDelegate;

	UPROPERTY()
		FInteractionNotificationDelegate LeftTriggerDelegate;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		UBaseVRInteractable * CurrentLeftHandInteraction;

	UPROPERTY(Category = Grabbing, BlueprintReadOnly)
		UBaseVRInteractable * CurrentRightHandInteraction;

	FVector CurrentMovementInput = FVector(0.0f, 0.0f, 0.0f);
};
