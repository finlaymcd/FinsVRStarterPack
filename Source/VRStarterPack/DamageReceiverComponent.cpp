// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageReceiverComponent.h"


// Sets default values for this component's properties
UDamageReceiverComponent::UDamageReceiverComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	DamageNotification.AddDynamic(this, &UDamageReceiverComponent::DamageTaken);
	// ...
}


// Called when the game starts
void UDamageReceiverComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDamageReceiverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDamageReceiverComponent::TakeDamage(float DamageAmount, AActor * DamageActor, FVector DamageLocation)
{
	if (LosesHealthOnDamage) {
		CurrentHealth -= DamageAmount;
	}
	DamageNotification.Broadcast(DamageAmount, DamageActor, DamageLocation);
}

void UDamageReceiverComponent::DamageTaken_Implementation(float DamageAmount, AActor * DamageActor, FVector DamageLocation)
{
}

