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
	InputComponent->BindAction("FaceButton1_R", IE_Pressed, this, &AVRPawn::RightFaceButtonOneDown);
	InputComponent->BindAction("FaceButton1_L", IE_Pressed, this, &AVRPawn::LeftFaceButtonOneDown);
	InputComponent->BindAction("FaceButton1_R", IE_Released, this, &AVRPawn::RightFaceButtonOneUp);
	InputComponent->BindAction("FaceButton1_L", IE_Released, this, &AVRPawn::LeftFaceButtonOneUp);
	InputComponent->BindAction("FaceButton2_R", IE_Pressed, this, &AVRPawn::RightFaceButtonTwoDown);
	InputComponent->BindAction("FaceButton2_L", IE_Pressed, this, &AVRPawn::LeftFaceButtonTwoDown);
	InputComponent->BindAction("FaceButton2_R", IE_Released, this, &AVRPawn::RightFaceButtonTwoUp);
	InputComponent->BindAction("FaceButton2_L", IE_Released, this, &AVRPawn::LeftFaceButtonTwoUp);
	InputComponent->BindAxis("HorizontalInput_L", this, &AVRPawn::CacheMovementInput_LX);
	InputComponent->BindAxis("VerticalInput_L", this, &AVRPawn::CacheMovementInput_LY);
	InputComponent->BindAxis("HorizontalInput_R", this, &AVRPawn::CacheMovementInput_RX);
	InputComponent->BindAxis("VerticalInput_R", this, &AVRPawn::CacheMovementInput_RY);
	InputComponent->BindAxis("Grip_L", this, &AVRPawn::InputLeftGrip);
	InputComponent->BindAxis("Grip_R", this, &AVRPawn::InputRightGrip);

}

void AVRPawn::CacheMovementInput_LX(float AxisInput)
{
	if (MovementOnLeftHand) {
		CurrentMovementInput.X += AxisInput;
	}
	else if (RotationOnLeftHand) {
		HandlePlayerRotation(AxisInput);
	}
}

void AVRPawn::CacheMovementInput_LY(float AxisInput)
{
	if (MovementOnLeftHand) {
		CurrentMovementInput.Y += AxisInput;
	}
}

void AVRPawn::CacheMovementInput_RX(float AxisInput)
{
	if (!MovementOnLeftHand) {
		CurrentMovementInput.X += AxisInput;
	}
	else if (!RotationOnLeftHand) {
		HandlePlayerRotation(AxisInput);
	}
}

void AVRPawn::CacheMovementInput_RY(float AxisInput)
{
	if (!MovementOnLeftHand) {
		CurrentMovementInput.Y += AxisInput;
	}
}

void AVRPawn::HandlePlayerRotation(float AxisInput)
{
	if (RotationType == ERotationSystemEnum::RoomScale) {
		return;
	}
	else if (RotationType == ERotationSystemEnum::SnapTurn) {
		if (!CanSnapTurn) { return; }
			if(AxisInput > 0.3f){
				AddControllerYawInput(SnapTurnAngle);
				CanSnapTurn = false;
				GetWorld()->GetTimerManager().SetTimer(SnapTurnDelayTimerHandle, this, &AVRPawn::ResetSnapTurn, SnapTurnDelay, false);
			}
			else if(AxisInput < -0.3f){
				AddControllerYawInput(SnapTurnAngle * -1.0f);
				CanSnapTurn = false;
				GetWorld()->GetTimerManager().SetTimer(SnapTurnDelayTimerHandle, this, &AVRPawn::ResetSnapTurn, SnapTurnDelay, false);
			}

	}
	else{
		AddControllerYawInput(SmoothTurnSpeed * AxisInput);
	}

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

void AVRPawn::RightFaceButtonOneDown()
{
	RightFaceButtonOneDelegate.Broadcast(RMotionController, 1.0f);
}

void AVRPawn::RightFaceButtonOneUp()
{
	RightFaceButtonOneDelegate.Broadcast(RMotionController, 0.0f);
}

void AVRPawn::LeftFaceButtonOneDown()
{
	LeftFaceButtonOneDelegate.Broadcast(LMotionController, 1.0f);
}

void AVRPawn::LeftFaceButtonOneUp()
{
	LeftFaceButtonOneDelegate.Broadcast(LMotionController, 0.0f);
}

void AVRPawn::RightFaceButtonTwoDown()
{
	UE_LOG(LogTemp, Warning, TEXT("RIGHT HAND SETUP"));
	RightFaceButtonTwoDelegate.Broadcast(RMotionController, 1.0f);
}

void AVRPawn::RightFaceButtonTwoUp()
{
	RightFaceButtonTwoDelegate.Broadcast(RMotionController, 0.0f);
}

void AVRPawn::LeftFaceButtonTwoDown()
{
	LeftFaceButtonTwoDelegate.Broadcast(LMotionController, 1.0f);
}

void AVRPawn::LeftFaceButtonTwoUp()
{
	LeftFaceButtonTwoDelegate.Broadcast(LMotionController, 0.0f);
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
				AttemptRelease(Box, MotionController);
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
	if (HandOverlap == LHandOverlap) {
		if (CurrentLeftHandInteraction != nullptr) {
			CurrentLeftHandInteraction = nullptr;
			LeftTriggerDelegate.Clear();
			LeftTriggerDelegate.RemoveAll(this);
		}
	}
	else {
		if (CurrentRightHandInteraction != nullptr) {
			CurrentRightHandInteraction = nullptr;
			RightTriggerDelegate.Clear();
			RightTriggerDelegate.RemoveAll(this);
		}
	}
}


void AVRPawn::SetupCurrentInteractionDelegates(bool LeftHand)
{
	FInteractionNotificationDelegate * TriggerDelegate = nullptr;
	FInteractionNotificationDelegate * FaceOneDelegate = nullptr;
	FInteractionNotificationDelegate * FaceTwoDelegate = nullptr;
	UBaseVRInteractable * Interactable = nullptr;

	if (LeftHand) {
		TriggerDelegate = &LeftTriggerDelegate;
		FaceOneDelegate = &LeftFaceButtonOneDelegate;
		FaceTwoDelegate = &LeftFaceButtonTwoDelegate;
		Interactable = CurrentLeftHandInteraction;
	}
	else {
		
		TriggerDelegate = &RightTriggerDelegate;
		FaceOneDelegate = &RightFaceButtonOneDelegate;
		FaceTwoDelegate = &RightFaceButtonTwoDelegate;
		Interactable = CurrentRightHandInteraction;
	}
	if (Interactable != nullptr) {
		if (Interactable->InteractButtonOne == EInteractButtonEnum::Trigger) {
			TriggerDelegate->AddDynamic(Interactable, &UBaseVRInteractable::InteractOne);
		}
		else if (Interactable->InteractButtonOne == EInteractButtonEnum::ButtonOne) {
			FaceOneDelegate->AddDynamic(Interactable, &UBaseVRInteractable::InteractOne);
		}
		else if (Interactable->InteractButtonOne == EInteractButtonEnum::ButtonTwo) {
			FaceTwoDelegate->AddDynamic(Interactable, &UBaseVRInteractable::InteractOne);
		}
		if (Interactable->InteractButtonTwo == EInteractButtonEnum::Trigger) {
			TriggerDelegate->AddDynamic(Interactable, &UBaseVRInteractable::InteractTwo);
		}
		else if (Interactable->InteractButtonTwo == EInteractButtonEnum::ButtonOne) {
			FaceOneDelegate->AddDynamic(Interactable, &UBaseVRInteractable::InteractTwo);
		}
		else if (Interactable->InteractButtonTwo == EInteractButtonEnum::ButtonTwo) {
			FaceTwoDelegate->AddDynamic(Interactable, &UBaseVRInteractable::InteractTwo);
		}
		if (Interactable->InteractButtonThree == EInteractButtonEnum::Trigger) {
			TriggerDelegate->AddDynamic(Interactable, &UBaseVRInteractable::InteractThree);
		}
		else if (Interactable->InteractButtonThree == EInteractButtonEnum::ButtonOne) {
			FaceOneDelegate->AddDynamic(Interactable, &UBaseVRInteractable::InteractThree);
		}
		else if (Interactable->InteractButtonThree == EInteractButtonEnum::ButtonTwo) {
			FaceTwoDelegate->AddDynamic(Interactable, &UBaseVRInteractable::InteractThree
			);
		}
	}
}

void AVRPawn::ResetSnapTurn()
{
	CanSnapTurn = true;
}

void AVRPawn::NotifyAttemptGrab_Implementation(USceneComponent * Hand, float Value)
{

}


