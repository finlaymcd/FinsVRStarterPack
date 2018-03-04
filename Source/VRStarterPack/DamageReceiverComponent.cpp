// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageReceiverComponent.h"


// Sets default values for this component's properties
UDamageReceiverComponent::UDamageReceiverComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	DamageNotification.AddDynamic(this, &UDamageReceiverComponent::DamageTaken);
	HealthDepletedDelegate.AddDynamic(this, &UDamageReceiverComponent::HealthDepletedNotification);
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

void UDamageReceiverComponent::TakeDamage(float DamageAmount, AActor * DamageActor, USceneComponent * DamageComponent, FVector DamageLocation)
{
	if (LosesHealthOnDamage) {
		CurrentHealth -= DamageAmount;
		if (CurrentHealth <= 0.0f && !Depleted) {
			HealthDepleted(DamageAmount, DamageActor, DamageComponent, DamageLocation);
		}
	}
	DamageNotification.Broadcast(DamageAmount, DamageActor, DamageComponent, DamageLocation);
}

void UDamageReceiverComponent::HealthDepleted(float DamageAmount, AActor * DamageActor, USceneComponent * DamageComponent, FVector DamageLocation)
{
	Depleted = true;
	HealthDepletedDelegate.Broadcast(DamageAmount, DamageActor, DamageComponent, DamageLocation);
}

void UDamageReceiverComponent::HealthDepletedNotification_Implementation(float DamageAmount, AActor * DamageActor, USceneComponent * DamageComponent, FVector DamageLocation)
{
}

void UDamageReceiverComponent::DamageTaken_Implementation(float DamageAmount, AActor * DamageActor, USceneComponent * DamageComponent, FVector DamageLocation)
{
}

