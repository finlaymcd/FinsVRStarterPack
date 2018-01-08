// Fill out your copyright notice in the Description page of Project Settings.

#include "RotationalInteractionComponent.h"

URotationalInteractionComponent::URotationalInteractionComponent() {
	//ZRotGimbal = CreateDefaultSubobject<USceneComponent>(TEXT("ZRotGimbal"), true);
	//ZRotGimbal->SetupAttachment(GetAttachParent());
	//this->SetupAttachment(ZRotGimbal);
}

void URotationalInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (DualAxisInteraction) {
		InitializeZRotGimbal();
	}
	SavedRelativePitch = GetRelativeTransform().Rotator().Pitch;
	SavedRelativeRoll = GetRelativeTransform().Rotator().Roll;
}

void URotationalInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	if (GrabDown) {
		UpdateCurrentInteraction();
	}
}

void URotationalInteractionComponent::UpdateCurrentInteraction()
{

	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), CurrentInteractingHand->GetComponentLocation());
	SetWorldRotation(LookAtRot);
	SetRelativeRotation(FRotator(SavedRelativePitch, GetRelativeTransform().Rotator().Yaw, SavedRelativeRoll));
	if (DualAxisInteraction && ZRotGimbal != nullptr) {

		FRotator ZLookAtRot = UKismetMathLibrary::FindLookAtRotation(ZRotGimbal->GetComponentLocation(), CurrentInteractingHand->GetComponentLocation());
		ZRotGimbal->SetWorldRotation(ZLookAtRot);
		ZRotGimbal->SetRelativeRotation(FRotator(ZGimbalSavedRelativePitch, ZRotGimbal->GetRelativeTransform().Rotator().Yaw, ZGimbalSavedRelativeRoll));
		UE_LOG(LogTemp, Warning, TEXT("Relative %f"), GetRelativeTransform().Rotator().Yaw);
	}

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
}
