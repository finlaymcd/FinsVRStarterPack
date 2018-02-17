// Fill out your copyright notice in the Description page of Project Settings.

#include "LinearInteractionComponent.h"


void ULinearInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	SavedRelativeXLocation = GetRelativeTransform().GetLocation().X;
	SavedRelativeYLocation = GetRelativeTransform().GetLocation().Y;
	SavedRelativeZLocation = GetRelativeTransform().GetLocation().Z;
}

void ULinearInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	if (GrabDown) {
		UpdateCurrentInteraction();
	}
}

void ULinearInteractionComponent::GrabOn(USceneComponent * Hand, USceneComponent * HandVisual, bool TeleGrab, bool LeftHand)
{
	Super::GrabOn(Hand, HandVisual, TeleGrab, LeftHand);
}

void ULinearInteractionComponent::GrabOff(USceneComponent * Hand)
{
	Super::GrabOff(Hand);
}

void ULinearInteractionComponent::UpdateCurrentInteraction()
{
	FVector Relative = GetRelativeTransform().GetLocation();
	SetWorldLocation(CurrentInteractingHand->GetComponentLocation());
	if(InteractOnX) {
		Relative.X = FMath::Clamp(GetRelativeTransform().GetLocation().X, XMinDist, XMaxDist);
	}
	if (InteractOnY) {
		Relative.Y = FMath::Clamp(GetRelativeTransform().GetLocation().Y, YMinDist, YMaxDist);
	}
	if (InteractOnZ) {
		Relative.Z = FMath::Clamp(GetRelativeTransform().GetLocation().Z,ZMinDist, ZMaxDist);
	}
	SetRelativeLocation(Relative);


}	
