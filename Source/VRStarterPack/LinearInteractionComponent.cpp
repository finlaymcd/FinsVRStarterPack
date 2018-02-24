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
		BlueprintInteractionUpdate.Broadcast(this, FVector(XInteractionPercentage, YInteractionPercentage, ZInteractionPercentage));
	}
}

void ULinearInteractionComponent::GrabOn(USceneComponent * Hand, USceneComponent * HandVisual, UInteractableHandComponent * HandLogic, bool TeleGrab, bool LeftHand)
{
	Super::GrabOn(Hand, HandVisual, HandLogic, TeleGrab, LeftHand);
	if (SnapHandMeshToInteraction && HandVisual != nullptr) {
		HandParentRelativeTransform = HandVisual->GetRelativeTransform();
		HandVisual->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform, "None");
		HandVisual->SetRelativeRotation(HandSnapRotation);
		CurrentHandVisual->SetRelativeLocation(HandSnapLocation);
	}
}

void ULinearInteractionComponent::GrabOff(USceneComponent * Hand)
{
	Super::GrabOff(Hand);
	if (SnapHandMeshToInteraction && CurrentHandVisual != nullptr && Hand == CurrentInteractingHand) {
		CurrentHandVisual->AttachToComponent(Hand, FAttachmentTransformRules::KeepWorldTransform, "None");
		CurrentHandVisual->SetRelativeTransform(HandParentRelativeTransform);
	}
}

void ULinearInteractionComponent::UpdateCurrentInteraction()
{
	FVector Relative = GetRelativeTransform().GetLocation();
	SetWorldLocation(CurrentInteractingHand->GetComponentLocation());
	if(InteractOnX) {
		Relative.X = FMath::Clamp(GetRelativeTransform().GetLocation().X, XMinDist, XMaxDist);
		XInteractionPercentage = CalculateInteractPercentage(XMinDist, XMaxDist, Relative.X);
		UE_LOG(LogTemp, Warning, TEXT("X INT PERCENT = %f"), XInteractionPercentage);
	}
	if (InteractOnY) {
		Relative.Y = FMath::Clamp(GetRelativeTransform().GetLocation().Y, YMinDist, YMaxDist);
		YInteractionPercentage = CalculateInteractPercentage(YMinDist, YMaxDist, Relative.Y);
	}
	if (InteractOnZ) {
		Relative.Z = FMath::Clamp(GetRelativeTransform().GetLocation().Z,ZMinDist, ZMaxDist);
		ZInteractionPercentage = CalculateInteractPercentage(ZMinDist, ZMaxDist, Relative.Z);
	}

	SetRelativeLocation(Relative);
}
float ULinearInteractionComponent::CalculateInteractPercentage(float min, float max, float current)
{
	float percentage = UKismetMathLibrary::InverseLerp(min, max, current);
	return percentage;
}
