// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableHandComponent.h"


// Sets default values for this component's properties
UInteractableHandComponent::UInteractableHandComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInteractableHandComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UInteractableHandComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (TeleportGrabType != EHandTeleGrabSystemEnum::NoTelegrab) {
		HandleTeleGrab();
	}
	// ...
}

/*Receiving input from input component*/
void UInteractableHandComponent::ReceiveFaceButtonOne(bool ButtonDown)
{
	if (ButtonDown) {
		FaceButtonOneDelegate.Broadcast(MotionController, 1.0f);
	}
	else {
		FaceButtonOneDelegate.Broadcast(MotionController, 0.0f);
	}
}

/*Receiving input from input component*/
void UInteractableHandComponent::ReceiveFaceButtonTwo(bool ButtonDown)
{
	if (ButtonDown) {
		FaceButtonTwoDelegate.Broadcast(MotionController, 1.0f);
	}
	else {
		FaceButtonTwoDelegate.Broadcast(MotionController, 0.0f);
	}
}

/*Receiving input from input component*/
void UInteractableHandComponent::ReceiveFaceButtonThree(bool ButtonDown)
{
	if (ButtonDown) {
		FaceButtonThreeDelegate.Broadcast(MotionController, 1.0f);
	}
	else {
		FaceButtonThreeDelegate.Broadcast(MotionController, 0.0f);
	}
}

/*Receiving input from input component*/
void UInteractableHandComponent::ReceiveTrigger(bool ButtonDown)
{
	if (ButtonDown) {
		TriggerDelegate.Broadcast(MotionController, 1.0f);
	}
	else {
		TriggerDelegate.Broadcast(MotionController, 0.0f);
	}
}

/*Receiving input from input component*/
void UInteractableHandComponent::ReceiveDualAxisInput(float X, float Y)
{

	DualAxisDelegate.Broadcast(MotionController, X, Y);
}

/*Handles realistic grabbing input*/
void UInteractableHandComponent::HandleRegularGrabInput(float AxisInput)
{

	if (!HandPastGrabThreshold) {
		if (AxisInput < GrabThreshold) {
			return;
		}
		else {
			AttemptGrab();
			HandPastGrabThreshold = true;
		}
	}
	else {
		if (AxisInput > GrabThreshold) {
			return;
		}
		else {
			HandPastGrabThreshold = false;
			AttemptRelease();
		}
	}
}

/*Handles snap grabbing input*/
void UInteractableHandComponent::HandleSnapGrabInput(float AxisInput, bool LeftHand)
{
	if (ListeningForSnapGrab && AxisInput > GrabThreshold) {
		ListeningForSnapGrab = false;
		if (CurrentlyGrabbed) {
			AttemptRelease();
		}
		else {
			AttemptGrab();
		}
	}
	else if (AxisInput < GrabThreshold) {
		ListeningForSnapGrab = true;
	}
}

/*Hand is gripped in the world and the hand overlap is checked for any interactables, and the closest one is grabbed. If there are none the cached telegrab object is checked, and grabbed*/
void UInteractableHandComponent::AttemptGrab()
{
	TArray<AActor*> Overlaps;
	TArray<UBaseVRInteractable*> Components;
	TArray<UBaseVRInteractable*> CollectedComponents;
	HandOverlap->GetOverlappingActors(Overlaps);
	UBaseVRInteractable * Interactable = nullptr;
	//USceneComponent * HandVisual = nullptr;
	bool TeleGrab = false;
	
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

	if (Interactable == nullptr) { //Do we still have nothing? Check the cached telegrab object then
		Interactable = CachedTeleGrabObject;
		TeleGrab = true;
	}

	if (Interactable != nullptr) { //Do we have an interactable? Grab it, save it, setup input delegates from controller to object
		Interactable->GrabOn(MotionController, HandVisual, this, TeleGrab, LeftHand);
		CurrentHandInteraction = Interactable;
		SetupCurrentInteractionDelegates();
		CurrentlyGrabbed = true;
	}
	else {
		CurrentlyGrabbed = false;
	}
	//GrabDelegate.Broadcast(Hand, 1.0f);

}

/*Hand is released and if it was holding something it is dropped and he hand is reverted to its default state*/
void UInteractableHandComponent::AttemptRelease()
{
		if (CurrentHandInteraction != nullptr) {
			CurrentHandInteraction->GrabOff(this->MotionController);
			CachedTeleGrabObject = nullptr;
			CurrentHandInteraction = nullptr;
			TriggerDelegate.Clear();
			TriggerDelegate.RemoveAll(this);
			FaceButtonOneDelegate.Clear();
			FaceButtonOneDelegate.RemoveAll(this);
			FaceButtonTwoDelegate.Clear();
			FaceButtonTwoDelegate.RemoveAll(this);
			FaceButtonThreeDelegate.Clear();
			FaceButtonThreeDelegate.RemoveAll(this);
			DualAxisDelegate.Clear();
			DualAxisDelegate.RemoveAll(this);
			SetTeleGrabType(TeleportGrabType, TeleGrabButton);
		}
	//GrabDelegate.Broadcast(Hand, 0.0f);
		CurrentlyGrabbed = false;
}

/*Save grip amount for animation*/
void UInteractableHandComponent::HandleAnimValues(float AxisValue)
{
	if (CurrentHandInteraction == nullptr) {
		CurrentGripAnimValue = AxisValue;
	}
	else {
		CurrentGripAnimValue = CurrentHandInteraction->AnimGrabValue;
	}
}

/*Filter different tele grab systems and trace for objects if appropriate*/
void UInteractableHandComponent::HandleTeleGrab()
{
	if (TeleportGrabType == EHandTeleGrabSystemEnum::AutoTelegrab) {
		CachedTeleGrabObject = TeleGrabLineTrace(MotionController, false);
	}
	else if (TeleportGrabType == EHandTeleGrabSystemEnum::ManualTelegrab) {
		if (CanTelegrab) {
			CachedTeleGrabObject = TeleGrabLineTrace(MotionController, true);
		}
	}
	if (CachedTeleGrabObject != nullptr) {
		CachedTeleGrabObject->OnHover(MotionController, true);
	}
}

/*Box trace from the forward vector of the input trace origin (usually a hand)*/
UBaseVRInteractable * UInteractableHandComponent::TeleGrabLineTrace(USceneComponent * TraceOrigin, bool DrawLine)
{
	FHitResult LineTraceHit;
	UBaseVRInteractable * Interactable = nullptr;
	FCollisionQueryParams TraceParams(FName(), false, GetOwner());


	GetWorld()->SweepSingleByObjectType(
		OUT LineTraceHit,
		TraceOrigin->GetComponentLocation(),
		TraceOrigin->GetComponentLocation() + (TraceOrigin->GetForwardVector() * TeleGrabMaxDistance),
		FQuat::Identity,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		FCollisionShape::MakeBox(FVector(20.0f, 20.0f, 20.0f)),
		TraceParams
	);

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

/*Links the inputs from the players controller to the held item in that hand*/
void UInteractableHandComponent::SetupCurrentInteractionDelegates()
{
	if (CurrentHandInteraction != nullptr) {
		TriggerDelegate.AddDynamic(CurrentHandInteraction, &UBaseVRInteractable::InteractOne);
		FaceButtonOneDelegate.AddDynamic(CurrentHandInteraction, &UBaseVRInteractable::InteractTwo);
		FaceButtonTwoDelegate.AddDynamic(CurrentHandInteraction, &UBaseVRInteractable::InteractThree);
		DualAxisDelegate.AddDynamic(CurrentHandInteraction, &UBaseVRInteractable::DualAxisInteractUpdate);
	}
}

/*Activates and deactivates telegrab trace, usually for manual tele grabbing*/
void UInteractableHandComponent::SetTelegrabTraceActive(USceneComponent * Hand, float Value)
{
	if (Value > 0.5f) {
		CanTelegrab = true;
	}
	else {
		CanTelegrab = false;
	}
}

/*Sets the telegrabbing system and if the system is manual, the button used to activate grabbing*/
void UInteractableHandComponent::SetTeleGrabType(EHandTeleGrabSystemEnum System, int InteractButtonForManual)
{
	TeleportGrabType = System;
	TeleGrabButton = InteractButtonForManual;
	if (System == EHandTeleGrabSystemEnum::ManualTelegrab) {
		switch (InteractButtonForManual)
		{
		case 1:
			//bind teleport activation to trigger
			TriggerDelegate.AddDynamic(this, &UInteractableHandComponent::SetTelegrabTraceActive);
			break;

		case 2:
			//bind teleport activation to face one
			FaceButtonOneDelegate.AddDynamic(this, &UInteractableHandComponent::SetTelegrabTraceActive);
			break;

		case 3:
			//bind teleport activation to face two
			FaceButtonTwoDelegate.AddDynamic(this, &UInteractableHandComponent::SetTelegrabTraceActive);
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("Teleport Grab Warning: No Interact button for manual provided in SetSnapGrabType"));
			break;
		}
	}
}

