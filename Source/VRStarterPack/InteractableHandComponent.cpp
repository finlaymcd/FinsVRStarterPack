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

void UInteractableHandComponent::AttemptGrab()
{
	TArray<AActor*> Overlaps;
	TArray<UBaseVRInteractable*> Components;
	TArray<UBaseVRInteractable*> CollectedComponents;
	HandOverlap->GetOverlappingActors(Overlaps);
	UBaseVRInteractable * Interactable = nullptr;
	USceneComponent * HandVisual = nullptr;
	bool TeleGrab = false;
	if (UseSkeletalMeshAsHands) {
		HandVisual = HandSkeletalMesh;
	}
	else {
		HandVisual = HandStaticMesh;
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
		Interactable = CachedTeleGrabObject;
		TeleGrab = true;
	}

	if (Interactable != nullptr) {
		Interactable->GrabOn(MotionController, HandVisual, this, TeleGrab, LeftHand);
		CurrentHandInteraction = Interactable;
		//SetupCurrentInteractionDelegates(true);
		CurrentlyGrabbed = true;
	}
	else {
		CurrentlyGrabbed = false;
	}
	//GrabDelegate.Broadcast(Hand, 1.0f);

}

void UInteractableHandComponent::AttemptRelease()
{
		if (CurrentHandInteraction != nullptr) {
			CurrentHandInteraction->GrabOff(this->MotionController);
			CachedTeleGrabObject = nullptr;
			CurrentHandInteraction = nullptr;
			//TriggerDelegate.Clear();
			//TriggerDelegate.RemoveAll(this);
			//FaceButtonOneDelegate.Clear();
			//FaceButtonOneDelegate.RemoveAll(this);
			//FaceButtonTwoDelegate.Clear();
			//FaceButtonTwoDelegate.RemoveAll(this);
			//InitializePawnControls();
		}
	//GrabDelegate.Broadcast(Hand, 0.0f);
		CurrentlyGrabbed = false;
}

void UInteractableHandComponent::HandleAnimValues(float AxisValue)
{
	if (CurrentHandInteraction == nullptr) {
		CurrentGripAnimValue = AxisValue;
	}
	else {
		CurrentGripAnimValue = CurrentHandInteraction->AnimGrabValue;
	}
}

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

