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

void UBaseVRInteractable::InteractOneOn(USceneComponent* Hand, float Value)
{
	InteractOneDown = true;
	InteractionNotification.Broadcast(EInteractType::InteractOne, true);
}

void UBaseVRInteractable::InteractOneOff(USceneComponent *Hand, float Value)
{
	InteractOneDown = false;
	InteractionNotification.Broadcast(EInteractType::InteractOne, false);
}

void UBaseVRInteractable::InteractTwoOn(USceneComponent *Hand, float Value)
{
	InteractTwoDown = true;
	InteractionNotification.Broadcast(EInteractType::InteractTwo, true);
}

void UBaseVRInteractable::InteractTwoOff(USceneComponent *Hand, float Value)
{
	InteractTwoDown = false;
	InteractionNotification.Broadcast(EInteractType::InteractTwo, false);
}

void UBaseVRInteractable::InteractThreeOn(USceneComponent *Hand, float Value)
{
	InteractThreeDown = true;
	InteractionNotification.Broadcast(EInteractType::InteractThree, true);
}

void UBaseVRInteractable::InteractThreeOff(USceneComponent *Hand, float Value)
{
	InteractThreeDown = false;
	InteractionNotification.Broadcast(EInteractType::InteractThree, false);
}

void UBaseVRInteractable::NotifyAttemptedInteraction_Implementation(EInteractType Interaction, bool InteractionDown)
{
	UE_LOG(LogTemp, Warning, TEXT("Noti"));
}




