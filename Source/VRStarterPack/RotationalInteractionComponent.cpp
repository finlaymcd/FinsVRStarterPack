// Fill out your copyright notice in the Description page of Project Settings.

#include "RotationalInteractionComponent.h"

URotationalInteractionComponent::URotationalInteractionComponent() {
	//ZRotGimbal = CreateDefaultSubobject<USceneComponent>(TEXT("ZRotGimbal"), true);
	//ZRotGimbal->SetupAttachment(GetAttachParent());
	//this->SetupAttachment(ZRotGimbal);
	BlueprintRotationInteractionUpdate.AddDynamic(this, &URotationalInteractionComponent::BlueprintUpdateRotValues);
}

void URotationalInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (DualAxisInteraction) {
		InitializeZRotGimbal();
	}
	SavedRelativePitch = GetRelativeTransform().Rotator().Pitch;
	SavedRelativeRoll = GetRelativeTransform().Rotator().Roll;
	XStartValue = GetRelativeTransform().Rotator().Yaw;
	
}

void URotationalInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	if (GrabDown) {
		UpdateCurrentInteraction();
		BlueprintRotationInteractionUpdate.Broadcast(this, XValue, ZValue);
	}
}

void URotationalInteractionComponent::GrabOn(USceneComponent * Hand, USceneComponent * HandVisual, UInteractableHandComponent * HandLogic, bool TeleGrab, bool LeftHand)
{
	Super::GrabOn(Hand, HandVisual, HandLogic, TeleGrab, LeftHand);
	if (SnapHandMeshToInteraction && HandVisual != nullptr) {
		HandParentRelativeTransform = HandVisual->GetRelativeTransform();
		HandVisual->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform, "None");
		HandVisual->SetRelativeRotation(HandSnapRotation);
		CurrentHandVisual->SetRelativeLocation(HandSnapLocation);
	}
}

void URotationalInteractionComponent::GrabOff(USceneComponent * Hand)
{
	Super::GrabOff(Hand);
	if (SnapHandMeshToInteraction && CurrentHandVisual != nullptr && Hand == CurrentInteractingHand) {
		CurrentHandVisual->AttachToComponent(Hand, FAttachmentTransformRules::KeepWorldTransform, "None");
		CurrentHandVisual->SetRelativeTransform(HandParentRelativeTransform);
	}
	if (ReturnToStartOnRelease) {
		ReturnToStartRotation();
	}
}

void URotationalInteractionComponent::UpdateCurrentInteraction()
{

	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), CurrentInteractingHand->GetComponentLocation());
	SetWorldRotation(LookAtRot);
	CurrentXAngle = FMath::Clamp(GetRelativeTransform().Rotator().Yaw, LowerAngleLimit, UpperAngleLimit);
	XValue = CalculateRotPercentage(LowerAngleLimit, UpperAngleLimit, CurrentXAngle);
	SetRelativeRotation(FRotator(SavedRelativePitch, CurrentXAngle, SavedRelativeRoll));
	if (DualAxisInteraction && ZRotGimbal != nullptr) {
		FRotator ZLookAtRot = UKismetMathLibrary::FindLookAtRotation(ZRotGimbal->GetComponentLocation(), CurrentInteractingHand->GetComponentLocation());
		ZRotGimbal->SetWorldRotation(ZLookAtRot);
		CurrentZAngle = FMath::Clamp(ZRotGimbal->GetRelativeTransform().Rotator().Yaw, ZLowerAngleLimit, ZUpperAngleLimit);
		ZValue = CalculateRotPercentage(ZLowerAngleLimit, ZUpperAngleLimit, CurrentZAngle);
		ZRotGimbal->SetRelativeRotation(FRotator(ZGimbalSavedRelativePitch, CurrentZAngle, ZGimbalSavedRelativeRoll));
	}
//	if (SnapHandMeshToInteraction) {
		//UpdateHandVisuals();
	//}

}

/*
void URotationalInteractionComponent::UpdateHandVisuals()
{
	if (CurrentHandVisual != nullptr) {
		CurrentHandVisual->SetRelativeLocation(HandSnapLocation);
	}

}
*/

float URotationalInteractionComponent::CalculateRotPercentage(float min, float max, float current)
{
	float percentage = UKismetMathLibrary::InverseLerp(min, max, current);
	return percentage;
}

void URotationalInteractionComponent::InitializeZRotGimbal()
{
	FName ObjectName("ZRotGimbal");
	USceneComponent* NewComp = NewObject<USceneComponent>(this, ObjectName);
	if (!NewComp)
	{
		return;
	}

	NewComp->RegisterComponent();
	NewComp->SetWorldLocation(GetComponentLocation());
	NewComp->SetWorldRotation(GetComponentRotation());
	NewComp->AttachToComponent(this->GetAttachParent(), FAttachmentTransformRules::KeepWorldTransform, "None");
	ZRotGimbal = NewComp;

	this->AttachToComponent(ZRotGimbal, FAttachmentTransformRules::KeepWorldTransform, "None");
	ZRotGimbal->AddRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	this->AddRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
	ZGimbalSavedRelativePitch = ZRotGimbal->GetRelativeTransform().Rotator().Pitch;
	ZGimbalSavedRelativeRoll = ZRotGimbal->GetRelativeTransform().Rotator().Roll;
	ZStartValue = ZRotGimbal->GetRelativeTransform().Rotator().Yaw;
}

void URotationalInteractionComponent::ReturnToStartRotation()
{
	SetRelativeRotation(FRotator(SavedRelativePitch, XStartValue, SavedRelativeRoll));
	if (DualAxisInteraction && ZRotGimbal != nullptr) {
		ZRotGimbal->SetRelativeRotation(FRotator(ZGimbalSavedRelativePitch, ZStartValue, ZGimbalSavedRelativeRoll));
	}
}

void URotationalInteractionComponent::BlueprintUpdateRotValues_Implementation(USceneComponent * RotComponent, float XValue, float YValue)
{

}
