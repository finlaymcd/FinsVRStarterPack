// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseVRInteractable.h"


// Sets default values for this component's properties
UBaseVRInteractable::UBaseVRInteractable()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	InteractionNotification.AddDynamic(this, &UBaseVRInteractable::NotifyAttemptedInteraction);
	// ...
}


// Called when the game starts
void UBaseVRInteractable::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBaseVRInteractable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBaseVRInteractable::GrabOn(USceneComponent* Hand)
{
	GrabDown = true;
	InteractionNotification.Broadcast(EInteractType::Grab, true);
}
void UBaseVRInteractable::GrabOff(USceneComponent* Hand)
{
	GrabDown = false;
	InteractionNotification.Broadcast(EInteractType::Grab, false);
}

void UBaseVRInteractable::InteractOne(USceneComponent* Hand, float Value)
{
	if (Value > 0.0f) {
		InteractOneDown = true;
		UE_LOG(LogTemp, Warning, TEXT("InteractOneDown"));
	}
	else {
		InteractOneDown = false;
		UE_LOG(LogTemp, Warning, TEXT("InteractOneUp"));
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
		UE_LOG(LogTemp, Warning, TEXT("InteractTwoDown"));
	}
	else {
		InteractTwoDown = false;
		UE_LOG(LogTemp, Warning, TEXT("InteractTwoUp"));
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
		UE_LOG(LogTemp, Warning, TEXT("InteractThreeDown"));
	}
	else {
		InteractThreeDown = false;
		UE_LOG(LogTemp, Warning, TEXT("InteractThreeUp"));
	}
	InteractionNotification.Broadcast(EInteractType::InteractThree, InteractThreeDown);
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




