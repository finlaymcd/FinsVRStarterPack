// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseVRInteractable.h"


// Sets default values for this component's properties
UBaseVRInteractable::UBaseVRInteractable()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

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

void UBaseVRInteractable::GrabOn(USceneComponent Hand)
{
	GrabDown = true;
}

void UBaseVRInteractable::GrabOff(USceneComponent Hand)
{
	GrabDown = false;
}

void UBaseVRInteractable::InteractOn(USceneComponent Hand)
{
	InteractDown = true;
}

void UBaseVRInteractable::InteractOff(USceneComponent Hand)
{
	InteractDown = false;
}

