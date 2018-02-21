// Fill out your copyright notice in the Description page of Project Settings.

#include "TwistInteractionComponent.h"

void UTwistInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeTrackingComponent();
	// ...

}

void UTwistInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	if (GrabDown) {
		UpdateCurrentInteraction();
	}
}

void UTwistInteractionComponent::GrabOn(USceneComponent * Hand, USceneComponent * HandVisual, bool TeleGrab, bool LeftHand)
{
	Super::GrabOn(Hand, HandVisual, TeleGrab, LeftHand);
	TwistTrackingComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	TwistTrackingComponent->AttachToComponent(Hand, FAttachmentTransformRules::KeepWorldTransform, "None");
	if (SnapHandMeshPosToInteraction && HandVisual != nullptr) {
		HandParentRelativeTransform = HandVisual->GetRelativeTransform();
		HandVisual->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform, "None");
		CurrentHandVisual->SetRelativeLocation(SnapHandVisualPosition);
		CurrentHandVisual->AddLocalOffset(FVector(-12.0f, 0.0f, 0.0f));
	}
}

void UTwistInteractionComponent::GrabOff(USceneComponent * Hand)
{
	Super::GrabOff(Hand);
	if (Hand == CurrentInteractingHand) {
		if (SnapHandMeshPosToInteraction && CurrentHandVisual != nullptr && Hand == CurrentInteractingHand) {
			CurrentHandVisual->AttachToComponent(Hand, FAttachmentTransformRules::KeepWorldTransform, "None");
			CurrentHandVisual->SetRelativeTransform(HandParentRelativeTransform);
		}
		TwistTrackingComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		TwistTrackingComponent->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform, "None");
		TwistTrackingComponent->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
	}
}

void UTwistInteractionComponent::UpdateCurrentInteraction()
{
	FRotator Look = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), TwistTrackingComponent->GetComponentLocation());
	SetWorldRotation(Look);
	SetRelativeRotation(FRotator(0.0f, GetRelativeTransform().Rotator().Yaw, 0.0f));

}

void UTwistInteractionComponent::InitializeTrackingComponent()
{
	FName ObjectName("TwistTrackingTarget");
	USceneComponent* NewComp = NewObject<USceneComponent>(this, ObjectName);
	if (!NewComp)
	{
		return;
	}

	NewComp->RegisterComponent();
	NewComp->SetWorldLocation(GetComponentLocation());
	NewComp->SetWorldRotation(GetComponentRotation());
	NewComp->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform, "None");
	TwistTrackingComponent = NewComp;
	
	TwistTrackingComponent->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
}
