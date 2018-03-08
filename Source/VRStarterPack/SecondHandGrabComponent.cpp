// Fill out your copyright notice in the Description page of Project Settings.

#include "SecondHandGrabComponent.h"

void USecondHandGrabComponent::BeginPlay()
{
	Super::BeginPlay();
	USceneComponent * Parent = GetAttachParent();
	if (Cast<UPickupObjectComponent>(Parent)) {
		PickupObjectParent = Cast<UPickupObjectComponent>(Parent);
	}
}

void USecondHandGrabComponent::GrabOn(USceneComponent* Hand, USceneComponent * HandVisual, UInteractableHandComponent * HandLogic, bool TeleGrab, bool LeftHand) 
{
	Super::GrabOn(Hand, HandVisual, HandLogic, TeleGrab, LeftHand);
	if (PickupObjectParent == nullptr) {
		return;
	}
	if (PickupObjectParent->CurrentlyGrabbed == true) {
		PickupObjectParent->ActivateRotationTarget(Hand, this);
	}
	else {
		PickupObjectParent->GrabOn(Hand, HandVisual, HandLogic, TeleGrab, LeftHand);
	}
}

void USecondHandGrabComponent::GrabOff(USceneComponent * Hand)
{
	Super::GrabOff(Hand);
	if (PickupObjectParent != nullptr) {
		PickupObjectParent->DeactivateRotationTarget();
	}
}