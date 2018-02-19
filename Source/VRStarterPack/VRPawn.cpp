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

	TeleportIndicator = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("TeleportHitIndicator"));
	TeleportIndicator->SetupAttachment(Root);
	TeleportIndicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
	GrabDelegate.AddDynamic(this, &AVRPawn::NotifyAttemptGrab);
	
	
}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();
	TeleportIndicator->SetVisibility(false);
	InitializePawnControls();
}

// Called every frame
void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ApplyCachedMovement();
	if (TeleportGrabType != ETeleGrabSystemEnum::NoTelegrab) {
		HandleTeleGrab();
	}
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
	HandleHandAnimValues(true, AxisInput);
}

void AVRPawn::InputRightGrip(float AxisInput)
{
	if (SnapGrab) {
		HandleSnapGrabInput(AxisInput, false);
	}
	else {
		HandleRegularGrabInput(AxisInput, false);
	}
	HandleHandAnimValues(false, AxisInput);
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
	UBaseVRInteractable * CachedTeleGrabInteractable = nullptr;
	float * AnimValueFloat = nullptr;
	if (LeftHand) {
		ThresholdBool = &LeftHandPastGrabThreshold;
		Box = LHandOverlap;
		MotionController = LMotionController;
		CachedTeleGrabInteractable = CachedTeleGrabObjectLeft;

	}
	else {
		ThresholdBool = &RightHandPastGrabThreshold;
		Box = RHandOverlap;
		MotionController = RMotionController;
		CachedTeleGrabInteractable = CachedTeleGrabObjectRight;

	}
		if (!*ThresholdBool) {
			if (AxisInput < GrabThreshold) {
				return;
			}
			else {
				AttemptGrab(Box, MotionController, CachedTeleGrabInteractable);
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
	bool * ThresholdBool = nullptr;
	bool * ListeningForGrab = nullptr;
	bool * CurrentlyGrabbed = nullptr;
	UBoxComponent * Box = nullptr;
	UMotionControllerComponent * MotionController = nullptr;
	UBaseVRInteractable * CachedTeleGrabInteractable = nullptr;
	if (LeftHand) {
		ThresholdBool = &LeftHandPastGrabThreshold;
		Box = LHandOverlap;
		MotionController = LMotionController;
		ListeningForGrab = &LeftListeningForSnapGrab;
		CurrentlyGrabbed = &LeftCurrentlyGrabbed;
		CachedTeleGrabInteractable = CachedTeleGrabObjectLeft;
	}
	else {
		ThresholdBool = &RightHandPastGrabThreshold;
		Box = RHandOverlap;
		MotionController = RMotionController;
		ListeningForGrab = &RightListeningForSnapGrab;
		CurrentlyGrabbed = &RightCurrentlyGrabbed;
		CachedTeleGrabInteractable = CachedTeleGrabObjectRight;
	}

	if (*ListeningForGrab && AxisInput > GrabThreshold) {
		*ListeningForGrab = false;
		if (*CurrentlyGrabbed) {
			AttemptRelease(Box, MotionController);
			*CurrentlyGrabbed = false;
		}
		else {
			AttemptGrab(Box, MotionController, CachedTeleGrabInteractable);
			*CurrentlyGrabbed = true;
		}
	}
	else if(AxisInput < GrabThreshold) {
		*ListeningForGrab = true;
	}

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

void AVRPawn::AttemptGrab(UBoxComponent * HandOverlap, UMotionControllerComponent * Hand, UBaseVRInteractable * CachedTelegrabObject)
{
	TArray<AActor*> Overlaps;
	TArray<UBaseVRInteractable*> Components;
	TArray<UBaseVRInteractable*> CollectedComponents;
	HandOverlap->GetOverlappingActors(Overlaps);
	UBaseVRInteractable * Interactable = nullptr;
	USceneComponent * HandVisual = nullptr;
	bool TeleGrab = false;
	bool LeftHand = true;
	if (Hand == RMotionController) {
		LeftHand = false;
		if (UseSkeletalMeshAsHands) {
			HandVisual = RHandSkeletalMesh;
		}
		else {
			HandVisual = RHandStaticMesh;
		}
	}
	else {
		if (UseSkeletalMeshAsHands) {
			HandVisual = LHandSkeletalMesh;
		}
		else {
			HandVisual = RHandStaticMesh;
		}
	}
	for (int i = 0; i < Overlaps.Num(); i++) { //For each actor that you've overlapped, collect the base interactables
		
		Overlaps[i]->GetComponents<UBaseVRInteractable>(Components);
			//Interactable = Components[0];
		for (int i = 0; i < Components.Num(); i++) { //add the interactables to the array
			CollectedComponents.Add(Components[i]);
			
		}
	}
	if (CollectedComponents.Num() > 0) {
		if (CollectedComponents.Num() == 1) {
			Interactable = CollectedComponents[0]; //If there's just one, set it as the interactable
			
		}
		else {
			float LowestDistance = 50000.0f;
			UBaseVRInteractable * Closest = nullptr;
			for (int i = 0; i < CollectedComponents.Num(); i++) { //if there's more than one, iterate through and compare locations
				float dist = FVector::Dist(CollectedComponents[i]->GetInteractableLocation(), HandOverlap->GetComponentLocation());
				if (dist < LowestDistance) {
					LowestDistance = dist;
					Closest = CollectedComponents[i];
				}
			}
			Interactable = Closest;
		}
	}

	if (Interactable == nullptr) {
			Interactable = CachedTelegrabObject;
			TeleGrab = true;
	}

	if (Interactable != nullptr) {
		Interactable->GrabOn(Hand, HandVisual, TeleGrab, LeftHand);
		if (Hand == LMotionController) {
			CurrentLeftHandInteraction = Interactable;
			SetupCurrentInteractionDelegates(true);
		}
		else {
			CurrentRightHandInteraction = Interactable;
			SetupCurrentInteractionDelegates(false);
		}
	}
	else {
		
	}
	GrabDelegate.Broadcast(Hand, 1.0f);
	

}

void AVRPawn::AttemptRelease(UBoxComponent * HandOverlap, UMotionControllerComponent * Hand)
{
	if (HandOverlap == LHandOverlap) {
		if (CurrentLeftHandInteraction != nullptr) {
			CurrentLeftHandInteraction->GrabOff(Hand);
			CachedTeleGrabObjectLeft = nullptr;
			CurrentLeftHandInteraction = nullptr;
			LeftTriggerDelegate.Clear();
			LeftTriggerDelegate.RemoveAll(this);
			LeftFaceButtonOneDelegate.Clear();
			LeftFaceButtonOneDelegate.RemoveAll(this);
			LeftFaceButtonTwoDelegate.Clear();
			LeftFaceButtonTwoDelegate.RemoveAll(this);
			InitializePawnControls();
		}
	}
	else {
		if (CurrentRightHandInteraction != nullptr) {
			CurrentRightHandInteraction->GrabOff(Hand);
			CachedTeleGrabObjectRight = nullptr;
			CurrentRightHandInteraction = nullptr;
			RightTriggerDelegate.Clear();
			RightTriggerDelegate.RemoveAll(this);
			RightFaceButtonOneDelegate.Clear();
			RightFaceButtonOneDelegate.RemoveAll(this);
			RightFaceButtonTwoDelegate.Clear();
			RightFaceButtonTwoDelegate.RemoveAll(this);
			InitializePawnControls();
		}
	}
	GrabDelegate.Broadcast(Hand, 0.0f);
}

void AVRPawn::HandleTeleGrab()
{
	if (TeleportGrabType == ETeleGrabSystemEnum::AutoTelegrab) {
		CachedTeleGrabObjectLeft = TeleGrabLineTrace(LMotionController, false);
		CachedTeleGrabObjectRight = TeleGrabLineTrace(RMotionController, false);
	}
	else if(TeleportGrabType == ETeleGrabSystemEnum::ManualTelegrab) {
		if (CanTelegrabLeft) {
			CachedTeleGrabObjectLeft = TeleGrabLineTrace(LMotionController, true);
		}
		if (CanTelegrabRight) {
			CachedTeleGrabObjectRight = TeleGrabLineTrace(RMotionController, true);
		}
	}
	if (CachedTeleGrabObjectLeft != nullptr) {
		CachedTeleGrabObjectLeft->OnHover(LMotionController, true);
	}
	if (CachedTeleGrabObjectRight != nullptr) {
		CachedTeleGrabObjectRight->OnHover(RMotionController, true);
	}
}

UBaseVRInteractable * AVRPawn::TeleGrabLineTrace(USceneComponent * TraceOrigin, bool DrawLine)
{
	
	FHitResult LineTraceHit;
	UBaseVRInteractable * Interactable = nullptr;
	FCollisionQueryParams TraceParams(FName(), false, GetOwner());

	if (BoxTraceForTelegrab) {
		GetWorld()->SweepSingleByObjectType(
			OUT LineTraceHit,
			TraceOrigin->GetComponentLocation(),
			TraceOrigin->GetComponentLocation() + (TraceOrigin->GetForwardVector() * TeleGrabMaxDistance),
			FQuat::Identity,
			FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
			FCollisionShape::MakeBox(FVector(20.0f, 20.0f, 20.0f)),
			TraceParams
		);
	}
	else {
		GetWorld()->LineTraceSingleByObjectType(
			OUT LineTraceHit,
			TraceOrigin->GetComponentLocation(),
			TraceOrigin->GetComponentLocation() + (TraceOrigin->GetForwardVector() * TeleGrabMaxDistance),
			FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
			TraceParams
		);
	}

	if (DrawLine) {
		DrawDebugLine(
			GetWorld(),
			TraceOrigin->GetComponentLocation(),
			TraceOrigin->GetComponentLocation() + (TraceOrigin->GetForwardVector() * TeleGrabMaxDistance),
			FColor::Green,
			false, -1, 0,
			1.0f
		); //draw visuals
	}
	if (LineTraceHit.Actor != nullptr) {
		TArray<UBaseVRInteractable*> Components;
		LineTraceHit.Actor->GetComponents<UBaseVRInteractable>(Components);
		if (Components.Num() > 0) {
			float LowestDistance = 50000.0f;
			UBaseVRInteractable * Closest = nullptr;
			for (int i = 0; i < Components.Num(); i++) { //if there's more than one, iterate through and compare locations
				float dist = FVector::Dist(Components[i]->GetInteractableLocation(), LineTraceHit.Location);
				if (dist < LowestDistance && Components[i]->CanTeleGrab) {
					LowestDistance = dist;
					Closest = Components[i];
				}
			}
			Interactable = Closest;
		}

	}
	return Interactable;

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
}



void AVRPawn::NotifyAttemptGrab_Implementation(USceneComponent * Hand, float Value)
{

}


