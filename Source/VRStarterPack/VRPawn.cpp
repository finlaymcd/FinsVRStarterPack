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

	LHandLogic = CreateDefaultSubobject<UInteractableHandComponent>(TEXT("HandLogicL"));
	LHandLogic->SetupAttachment(LMotionController);
	RHandLogic = CreateDefaultSubobject<UInteractableHandComponent>(TEXT("HandLogicR"));
	RHandLogic->SetupAttachment(RMotionController);

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

	TeleportIndicator = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("TeleportHitIndicator"));
	TeleportIndicator->SetupAttachment(Root);
	TeleportIndicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
	GrabDelegate.AddDynamic(this, &AVRPawn::NotifyAttemptGrab);
	
	LHandLogic->HandStaticMesh = LHandStaticMesh;
	LHandLogic->HandSkeletalMesh = LHandSkeletalMesh;
	LHandLogic->HandOverlap = LHandOverlap;
	LHandLogic->MotionController = LMotionController;
	LHandLogic->LeftHand = true;

	RHandLogic->HandStaticMesh = RHandStaticMesh;
	RHandLogic->HandSkeletalMesh = RHandSkeletalMesh;
	RHandLogic->HandOverlap = RHandOverlap;
	RHandLogic->MotionController = RMotionController;
	RHandLogic->LeftHand = false;
	InitializeHandValues(LHandLogic);
	InitializeHandValues(RHandLogic);
}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();
	TeleportIndicator->SetVisibility(false);
	InitializePawnControls();
	InitializeHandValues(LHandLogic);
	InitializeHandValues(RHandLogic);
}

// Called every frame
void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ApplyCachedMovement();
	LHandLogic->ReceiveDualAxisInput(LX, LY);
	RHandLogic->ReceiveDualAxisInput(RX, RY);
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

void AVRPawn::InitializeHandValues(UInteractableHandComponent * Hand)
{
	Hand->TeleGrabMaxDistance = TeleGrabMaxDistance;
	Hand->UseSkeletalMeshAsHands = UseSkeletalMeshAsHands;
	if (UseSkeletalMeshAsHands) {
		Hand->HandVisual = Hand->HandSkeletalMesh;
	}
	else {
		Hand->HandVisual = Hand->HandStaticMesh;
	}
	Hand->TeleportGrabType = TeleportGrabType;
	if (Hand == LHandLogic) {
		Hand->LeftHand = true;
	}
	else {
		Hand->LeftHand = false;
	}


}

void AVRPawn::InitializePawnControls()
{
	if (ManualTelegrabButton == EInteractButtonEnum::ButtonOne) {
		LeftFaceButtonOneDelegate.AddDynamic(this, &AVRPawn::SetTelegrabTraceActive);
		RightFaceButtonOneDelegate.AddDynamic(this, &AVRPawn::SetTelegrabTraceActive);
	}
	if (ManualTelegrabButton == EInteractButtonEnum::ButtonTwo) {
		LeftFaceButtonTwoDelegate.AddDynamic(this, &AVRPawn::SetTelegrabTraceActive);
		RightFaceButtonTwoDelegate.AddDynamic(this, &AVRPawn::SetTelegrabTraceActive);
	}
	if (ManualTelegrabButton == EInteractButtonEnum::Trigger) {
		LeftTriggerDelegate.AddDynamic(this, &AVRPawn::SetTelegrabTraceActive);
		RightTriggerDelegate.AddDynamic(this, &AVRPawn::SetTelegrabTraceActive);
	}

	if (UseSkeletalMeshAsHands) {
		RHandStaticMesh->SetVisibility(false);
		LHandStaticMesh->SetVisibility(false);
	}
	else {
		RHandSkeletalMesh->SetVisibility(false);
		LHandSkeletalMesh->SetVisibility(false);
	}
}

void AVRPawn::CacheMovementInput_LX(float AxisInput)
{
	LX = AxisInput;
	if (MovementOnLeftHand) {
		CurrentMovementInput.X += AxisInput;
	}
	else if (RotationOnLeftHand) {
		HandlePlayerRotation(AxisInput);
	}
}

void AVRPawn::CacheMovementInput_LY(float AxisInput)
{
	float LY = AxisInput;
	if (MovementOnLeftHand) {
		CurrentMovementInput.Y += AxisInput;
	}
}

void AVRPawn::CacheMovementInput_RX(float AxisInput)
{
	float RX = AxisInput;
	if (!MovementOnLeftHand) {
		CurrentMovementInput.X += AxisInput;
	}
	else if (!RotationOnLeftHand) {
		HandlePlayerRotation(AxisInput);
	}
}

void AVRPawn::CacheMovementInput_RY(float AxisInput)
{
	float RY = AxisInput;
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
		LHandLogic->HandleSnapGrabInput(AxisInput, true);
	}
	else {
		LHandLogic->HandleRegularGrabInput(AxisInput);
	}
	LHandLogic->HandleAnimValues(AxisInput);
}

void AVRPawn::InputRightGrip(float AxisInput)
{
	if (SnapGrab) {
		RHandLogic->HandleSnapGrabInput(AxisInput, false);
	}
	else {
		RHandLogic->HandleRegularGrabInput(AxisInput);
	}
	RHandLogic->HandleAnimValues(AxisInput);
}

void AVRPawn::LeftTriggerActionDown()
{
	//LeftTriggerDelegate.Broadcast(LMotionController, 1.0f);
	LHandLogic->ReceiveTrigger(true);
}

void AVRPawn::RightTriggerActionDown()
{
	//RightTriggerDelegate.Broadcast(RMotionController, 1.0f);
	RHandLogic->ReceiveTrigger(true);
}

void AVRPawn::LeftTriggerActionUp()
{
	//LeftTriggerDelegate.Broadcast(LMotionController, 0.0f);
	LHandLogic->ReceiveTrigger(false);
}

void AVRPawn::RightTriggerActionUp()
{
	//RightTriggerDelegate.Broadcast(RMotionController, 0.0f);
	RHandLogic->ReceiveTrigger(false);
}

void AVRPawn::RightFaceButtonOneDown()
{
	//RightFaceButtonOneDelegate.Broadcast(RMotionController, 1.0f);
	RHandLogic->ReceiveFaceButtonOne(true);
}

void AVRPawn::RightFaceButtonOneUp()
{
	//RightFaceButtonOneDelegate.Broadcast(RMotionController, 0.0f);
	RHandLogic->ReceiveFaceButtonOne(false);
}

void AVRPawn::LeftFaceButtonOneDown()
{
	//LeftFaceButtonOneDelegate.Broadcast(LMotionController, 1.0f);
	LHandLogic->ReceiveFaceButtonOne(true);
}

void AVRPawn::LeftFaceButtonOneUp()
{
	//LeftFaceButtonOneDelegate.Broadcast(LMotionController, 0.0f);
	LHandLogic->ReceiveFaceButtonOne(false);
}

void AVRPawn::RightFaceButtonTwoDown()
{
	//RightFaceButtonTwoDelegate.Broadcast(RMotionController, 1.0f);
	RHandLogic->ReceiveFaceButtonTwo(true);
}

void AVRPawn::RightFaceButtonTwoUp()
{
	//RightFaceButtonTwoDelegate.Broadcast(RMotionController, 0.0f);
	RHandLogic->ReceiveFaceButtonTwo(false);
}

void AVRPawn::LeftFaceButtonTwoDown()
{
	//LeftFaceButtonTwoDelegate.Broadcast(LMotionController, 1.0f);
	LHandLogic->ReceiveFaceButtonTwo(true);
}

void AVRPawn::LeftFaceButtonTwoUp()
{
	//LeftFaceButtonTwoDelegate.Broadcast(LMotionController, 0.0f);
	LHandLogic->ReceiveFaceButtonTwo(false);
}

void AVRPawn::ApplyCachedMovement()
{
	if (MovementType == EMovementSystemEnum::RoomScale) {
		return;
	}
	if (MovementType == EMovementSystemEnum::FreeLoco) {
		FVector FinalMovementInput = (LMotionController->GetForwardVector() * CurrentMovementInput.Y) + (LMotionController->GetRightVector() * CurrentMovementInput.X);
		FinalMovementInput.Z = 0;
		AddMovementInput(FinalMovementInput, MoveSpeed, false);
	}
	else {
		if (CurrentMovementInput.Y > 0.8f) {
			TeleportButtonDown = true;
		}
		else {
			TeleportButtonDown = false;
		}
		if (DrawingTeleportArc) {
			if (TeleportButtonDown) {
				//Keep Drawing Teleport Arc
				DrawTeleportArc();
				if (TeleportLocationIsValid) {
					TeleportIndicator->SetVisibility(true);
					TeleportIndicator->SetWorldLocation(CachedTeleportLocation);
				}
			}
			else {
				if (TeleportLocationIsValid) {
					TeleportPlayer();
				}
				DrawingTeleportArc = false;
			}
		}
		else {
			TeleportIndicator->SetVisibility(false);
			if (TeleportButtonDown) {
				DrawingTeleportArc = true;
			}
		}
	}
	
}

void AVRPawn::TeleportPlayer()
{
	SetActorLocation(CachedTeleportLocation + FVector(0.0f, 0.0f, 60.0f));
}

void AVRPawn::DrawTeleportArc()
{
	FVector StartLocation = FVector::ZeroVector;
	FVector Forward = FVector::ZeroVector;
	if (MovementOnLeftHand) {
		StartLocation = LMotionController->GetComponentLocation();
		Forward = LMotionController->GetForwardVector();
	}
	else {
		StartLocation = LMotionController->GetComponentLocation();
		Forward = LMotionController->GetForwardVector();
	}
	FVector EndLocation = StartLocation + (Forward * TeleportDistance);
	for (int i = 0; i < 10; i++) {
		FHitResult LineTraceHit;
		FCollisionQueryParams TraceParams(FName(), false, GetOwner());
		GetWorld()->LineTraceSingleByObjectType(
			OUT LineTraceHit,
			StartLocation,
			EndLocation,
			FCollisionObjectQueryParams(ECollisionChannel::ECC_GameTraceChannel1),
			TraceParams
		); //Trace for teleport object

		DrawDebugLine(
			GetWorld(),
			StartLocation,
			EndLocation,
			FColor::Cyan,
			false, -1, 0,
			12.333
		); //draw visuals

		if (LineTraceHit.Actor != nullptr) {
			CachedTeleportLocation = LineTraceHit.Location;
			TeleportLocationIsValid = true;
			break;
		} // if object hit, break loop, cache location
		else {
			Forward = EndLocation - StartLocation;
			Forward.Normalize();
			StartLocation = EndLocation;
			EndLocation = StartLocation + (Forward * TeleportDistance);
			float gravMultiplier = EndLocation.Z - StartLocation.Z;
			gravMultiplier = FMath::Abs(gravMultiplier);
			gravMultiplier *= 0.1f;
			EndLocation -= FVector(0.0f, 0.0f, 10.f + (10.0f * gravMultiplier));
			TeleportLocationIsValid = false;
		} //else, prepare value for next arc point
	}
}

void AVRPawn::SetTelegrabTraceActive(USceneComponent * Hand, float Value)
{
	if (Hand == LMotionController) {
		if (Value > 0.5f) {
			CanTelegrabLeft = true;
		}
		else {
			CanTelegrabLeft = false;
		}
	}
	else {
		if (Value > 0.5f) {
			CanTelegrabRight = true;
		}
		else {
			CanTelegrabRight = false;
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

void AVRPawn::HandleHandAnimValues(bool LeftHand, float AxisValue)
{
	/*
	UBaseVRInteractable * Interactable = nullptr;
	float * AnimValue = nullptr;
	if (LeftHand) {
		Interactable = CurrentLeftHandInteraction;
		AnimValue = &CurrentGripAnimValue_L;
	}
	else {
		Interactable = CurrentRightHandInteraction;
		AnimValue = &CurrentGripAnimValue_R;
	}
	if (Interactable == nullptr) {
		*AnimValue = AxisValue;
	}
	else {
		*AnimValue = Interactable->AnimGrabValue;
	}
	*/
	LHandLogic->HandleAnimValues(AxisValue);
	RHandLogic->HandleAnimValues(AxisValue);
}



void AVRPawn::NotifyAttemptGrab_Implementation(USceneComponent * Hand, float Value)
{

}


