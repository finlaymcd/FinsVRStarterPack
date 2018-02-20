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
	TwistTrackingComponent->AttachToComponent(HandVisual, FAttachmentTransformRules::KeepWorldTransform, "None");
}

void UTwistInteractionComponent::GrabOff(USceneComponent * Hand)
{
	Super::GrabOff(Hand);
	TwistTrackingComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	TwistTrackingComponent->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform, "None");
	TwistTrackingComponent->SetRelativeLocation(FVector(1000.0f, 0.0f, 0.0f));
}

void UTwistInteractionComponent::UpdateCurrentInteraction()
{
	FRotator Look = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), TwistTrackingComponent->GetComponentLocation());
	SetRelativeRotation(Look);
	SetRelativeRotation(FRotator(0.0f, Look.Yaw, 0.0f));

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
	
	TwistTrackingComponent->SetRelativeLocation(FVector(0.0f, 250.0f, 0.0f));
}
