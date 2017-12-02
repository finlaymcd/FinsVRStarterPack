// Fill out your copyright notice in the Description page of Project Settings.

#include "VRPawn.h"


// Sets default values
AVRPawn::AVRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PlayerCapsuleRoot"));
	SetRootComponent(Root); //Setup capsule as root

	TrackingOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("TrackingOrigin")); //For resetting camera position
	TrackingOrigin->SetupAttachment(RootComponent);

	RMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_R")); //initialies motion controllers
	RMotionController->SetupAttachment(TrackingOrigin);
	RMotionController->Hand = EControllerHand::Right;
	LMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_L"));
	LMotionController->SetupAttachment(TrackingOrigin);

	RHandStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightHandStatic")); //static meshes initialised and attached to hands
	RHandStaticMesh->SetupAttachment(RMotionController);
	LHandStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftHandStatic"));
	LHandStaticMesh->SetupAttachment(LMotionController);

	RHandSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHandSkeletal")); //skeletal meshes initialised and attached to hands
	RHandSkeletalMesh->SetupAttachment(RMotionController);
	LHandSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandSkeletal"));
	LHandSkeletalMesh->SetupAttachment(LMotionController);

	LHandOverlap = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandCollisionDetection"));
	LHandOverlap->SetupAttachment(LMotionController);
	RHandOverlap = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandCollisionDetection"));
	RHandOverlap->SetupAttachment(RMotionController);


	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	PlayerCamera->SetupAttachment(TrackingOrigin);

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
	GrabDelegate.AddDynamic(this, &AVRPawn::NotifyAttemptGrab);
	
	
}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ApplyCachedMovement();
	CurrentMovementInput = FVector::ZeroVector;
	
}

// Called to bind functionality to input
void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAction("Trigger_L", IE_Pressed, this, &AVRPawn::LeftTriggerActionDown);
	InputComponent->BindAction("Trigger_R", IE_Pressed, this, &AVRPawn::RightTriggerActionDown);
	InputComponent->BindAction("Trigger_L", IE_Released, this, &AVRPawn::LeftTriggerActionUp);
	InputComponent->BindAction("Trigger_R", IE_Released, this, &AVRPawn::RightTriggerActionUp);
	InputComponent->BindAxis("HorizontalInput_L", this, &AVRPawn::CacheMovementInput_LX);
	InputComponent->BindAxis("VerticalInput_L", this, &AVRPawn::CacheMovementInput_LY);
	InputComponent->BindAxis("Grip_L", this, &AVRPawn::InputLeftGrip);
	InputComponent->BindAxis("Grip_R", this, &AVRPawn::InputRightGrip);

}

void AVRPawn::CacheMovementInput_LX(float AxisInput)
{
	CurrentMovementInput.X += AxisInput;
}

void AVRPawn::CacheMovementInput_LY(float AxisInput)
{
	CurrentMovementInput.Y += AxisInput;
}

void AVRPawn::InputLeftGrip(float AxisInput)
{
	if (SnapGrab) {
		HandleSnapGrabInput(AxisInput, true);
	}
	else {
		HandleRegularGrabInput(AxisInput, true);
	}
}

void AVRPawn::InputRightGrip(float AxisInput)
{
	if (SnapGrab) {
		HandleSnapGrabInput(AxisInput, false);
	}
	else {
		HandleRegularGrabInput(AxisInput, false);
	}
}

void AVRPawn::LeftTriggerActionDown()
{
	LeftTriggerDelegate.Broadcast(LMotionController, 1.0f);
}

void AVRPawn::RightTriggerActionDown()
{
	RightTriggerDelegate.Broadcast(RMotionController, 1.0f);
}

void AVRPawn::LeftTriggerActionUp()
{
	LeftTriggerDelegate.Broadcast(LMotionController, 0.0f);
}

void AVRPawn::RightTriggerActionUp()
{
	RightTriggerDelegate.Broadcast(RMotionController, 0.0f);
}

void AVRPawn::HandleRegularGrabInput(float AxisInput, bool LeftHand)
{
	bool * ThresholdBool = nullptr;
	UBoxComponent * Box = nullptr;
	UMotionControllerComponent * MotionController = nullptr;
	if (LeftHand) {
		ThresholdBool = &LeftHandPastGrabThreshold;
		Box = LHandOverlap;
		MotionController = LMotionController;
	}
	else {
		ThresholdBool = &RightHandPastGrabThreshold;
		Box = RHandOverlap;
		MotionController = RMotionController;
	}
		if (!*ThresholdBool) {
			if (AxisInput < GrabThreshold) {
				return;
			}
			else {
				AttemptGrab(Box, MotionController);
				*ThresholdBool = true;
			}
		}
		else {
			if (AxisInput > GrabThreshold) {
				return;
			}
			else {
				*ThresholdBool = false;
				AttemptRelease(Box, LMotionController);
			}
		}
	
}

void AVRPawn::HandleSnapGrabInput(float AxisInput, bool LeftHand)
{
}

void AVRPawn::ApplyCachedMovement()
{
	FVector FinalMovementInput = (LMotionController->GetForwardVector() * CurrentMovementInput.Y) + (LMotionController->GetRightVector() * CurrentMovementInput.X);
	FinalMovementInput.Z = 0;
	AddMovementInput(FinalMovementInput, MoveSpeed, false);
}

void AVRPawn::AttemptGrab(UBoxComponent * HandOverlap, UMotionControllerComponent * Hand)
{
	TArray<AActor*> Overlaps;
	TArray<UBaseVRInteractable*> Components;
	HandOverlap->GetOverlappingActors(Overlaps);
	UBaseVRInteractable * Interactable = nullptr;
	for (int i = 0; i < Overlaps.Num(); i++) {
		Overlaps[i]->GetComponents<UBaseVRInteractable>(Components);
		if (Components.Num() > 0) {
			Interactable = Components[0];
		}
	}

	if (Interactable != nullptr) {
		Interactable->GrabOn(Hand);
		if (Hand == LMotionController) {
			CurrentLeftHandInteraction = Interactable;
			SetupCurrentInteractionDelegates(true);
		}
		else {
			CurrentRightHandInteraction = Interactable;
			SetupCurrentInteractionDelegates(false);
		}
	}
	GrabDelegate.Broadcast(Hand, 1.0f);
	

}

void AVRPawn::AttemptRelease(UBoxComponent * HandOverlap, UMotionControllerComponent * Hand)
{
	UE_LOG(LogTemp, Warning, TEXT("1"));
	if (HandOverlap == LHandOverlap) {
		UE_LOG(LogTemp, Warning, TEXT("2"));
		if (CurrentLeftHandInteraction != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("AttemptReleaseLeft"));
			CurrentLeftHandInteraction = nullptr;
			LeftTriggerDelegate.Clear();
			LeftTriggerDelegate.RemoveAll(this);
		}
	}
	else {
		if (CurrentRightHandInteraction != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("AttemptReleaseRight"));
			CurrentRightHandInteraction = nullptr;
			RightTriggerDelegate.Clear();
			RightTriggerDelegate.RemoveAll(this);
		}
	}
}


void AVRPawn::SetupCurrentInteractionDelegates(bool LeftHand)
{
	if (LeftHand && CurrentLeftHandInteraction != nullptr) {
		if (CurrentLeftHandInteraction->InteractButtonOne == EInteractButtonEnum::Trigger) {
			LeftTriggerDelegate.AddDynamic(CurrentLeftHandInteraction, &UBaseVRInteractable::InteractOne);
		}
		if (CurrentLeftHandInteraction->InteractButtonTwo == EInteractButtonEnum::Trigger) {
			LeftTriggerDelegate.AddDynamic(CurrentLeftHandInteraction, &UBaseVRInteractable::InteractTwo);
		}
		if (CurrentLeftHandInteraction->InteractButtonThree == EInteractButtonEnum::Trigger) {
			LeftTriggerDelegate.AddDynamic(CurrentLeftHandInteraction, &UBaseVRInteractable::InteractThree);
		}
	}
	if (!LeftHand && CurrentRightHandInteraction != nullptr) {
		if (CurrentRightHandInteraction->InteractButtonOne == EInteractButtonEnum::Trigger) {
			RightTriggerDelegate.AddDynamic(CurrentRightHandInteraction, &UBaseVRInteractable::InteractOne);
		}
		if (CurrentRightHandInteraction->InteractButtonTwo == EInteractButtonEnum::Trigger) {
			RightTriggerDelegate.AddDynamic(CurrentRightHandInteraction, &UBaseVRInteractable::InteractTwo);
		}
		if (CurrentRightHandInteraction->InteractButtonThree == EInteractButtonEnum::Trigger) {
			RightTriggerDelegate.AddDynamic(CurrentRightHandInteraction, &UBaseVRInteractable::InteractThree);
		}
	}
}

void AVRPawn::NotifyAttemptGrab_Implementation(USceneComponent * Hand, float Value)
{

}


