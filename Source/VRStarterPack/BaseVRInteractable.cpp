// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseVRInteractable.h"
#include "InteractableHandComponent.h"

// Sets default values for this component's properties
UBaseVRInteractable::UBaseVRInteractable()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	InteractionNotification.AddDynamic(this, &UBaseVRInteractable::NotifyAttemptedInteraction);
	StartingRootComponent = GetAttachParent();
	// ...
}


// Called when the game starts
void UBaseVRInteractable::BeginPlay()
{
	Super::BeginPlay();
	StartingRootComponent = GetAttachParent();
	// ...
	
}


// Called every frame
void UBaseVRInteractable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBaseVRInteractable::GrabOn(USceneComponent* Hand, USceneComponent * HandVisual, UInteractableHandComponent * HandLogic, bool TeleGrab, bool LeftHand)
{
	if (CurrentInteractingHand != nullptr && CurrentHandLogic->CurrentHandInteraction == this) {
		//GrabOff(CurrentInteractingHand);
		CurrentHandLogic->AttemptRelease();
	}

	GrabDown = true;
	CurrentInteractingHand = Hand;
	CurrentHandVisual = HandVisual;
	CurrentHandLogic = HandLogic;
	InteractionNotification.Broadcast(EInteractType::Grab, true);
	
}

void UBaseVRInteractable::GrabOff(USceneComponent* Hand)
{
	if (Hand == CurrentInteractingHand) {
		GrabDown = false;
		InteractionNotification.Broadcast(EInteractType::Grab, false);
	}


}

void UBaseVRInteractable::InteractOne(USceneComponent* Hand, float Value)
{
	if (Value > 0.0f) {
		InteractOneDown = true;
	}
	else {
		InteractOneDown = false;
	}
	InteractionNotification.Broadcast(EInteractType::InteractOne, InteractOneDown);
}
/*
void UBaseVRInteractable::InteractOneOff(USceneComponent *Hand, float Value)
{
	InteractOneDown = false;
	InteractionNotification.Broadcast(EInteractType::InteractOne, false);
}
*/

void UBaseVRInteractable::InteractTwo(USceneComponent *Hand, float Value)
{
	if (Value > 0.0f) {
		InteractTwoDown = true;

	}
	else {
		InteractTwoDown = false;

	}
	InteractionNotification.Broadcast(EInteractType::InteractTwo, InteractTwoDown);
}

/*
void UBaseVRInteractable::InteractTwoOff(USceneComponent *Hand, float Value)
{
	InteractTwoDown = false;
	InteractionNotification.Broadcast(EInteractType::InteractTwo, false);
}
*/

void UBaseVRInteractable::InteractThree(USceneComponent *Hand, float Value)
{
	if (Value > 0.0f) {
		InteractThreeDown = true;
	}
	else {
		InteractThreeDown = false;
	}
	InteractionNotification.Broadcast(EInteractType::InteractThree, InteractThreeDown);
}

void UBaseVRInteractable::DualAxisInteractUpdate(USceneComponent * Hand, float XValue, float YValue)
{

}

void UBaseVRInteractable::OnHover(USceneComponent * Hand, bool Telegrab)
{
	
}

FVector UBaseVRInteractable::GetInteractableLocation()
{
	return GetComponentLocation();
}


/*
void UBaseVRInteractable::InteractThreeOff(USceneComponent *Hand, float Value)
{
	InteractThreeDown = false;
	InteractionNotification.Broadcast(EInteractType::InteractThree, false);
}

*/
void UBaseVRInteractable::NotifyAttemptedInteraction_Implementation(EInteractType Interaction, bool InteractionDown)
{
	
}




