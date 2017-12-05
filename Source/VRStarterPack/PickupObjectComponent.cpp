// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupObjectComponent.h"

void UPickupObjectComponent::GrabOn(USceneComponent * Hand)
{
	Super::GrabOn(Hand);
	if (PhysicsObject) {
		UStaticMeshComponent * ParentMesh = Cast<UStaticMeshComponent>(GetAttachParent());
		ParentMesh->SetSimulatePhysics(false);
	}
	GetOwner()->AttachToComponent(Hand, FAttachmentTransformRules::KeepWorldTransform, "None");
}

void UPickupObjectComponent::GrabOff(USceneComponent * Hand)
{
	Super::GrabOff(Hand);
	if (GetOwner() != GetAttachmentRootActor() && Hand == CurrentInteractingHand) {
		GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		if (PhysicsObject) {
			UStaticMeshComponent * ParentMesh = Cast<UStaticMeshComponent>(GetAttachParent());
			ParentMesh->SetSimulatePhysics(true);
		}
	}
}
