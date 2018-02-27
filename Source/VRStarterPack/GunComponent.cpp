// Fill out your copyright notice in the Description page of Project Settings.

#include "GunComponent.h"


// Sets default values for this component's properties
UGunComponent::UGunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	GunShotNotify.AddDynamic(this, &UGunComponent::ShotFired);
	// ...
}


// Called when the game starts
void UGunComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGunComponent::PullTrigger()
{
	TriggerDown = true;
	if (ShotChambered) {
		FireShot();
	}
}

void UGunComponent::ReleaseTrigger()
{
	TriggerDown = false;
	if (GunType == EGunType::SemiAutomatic) {
		AttemptChamberShot();
	}
}

FShotDataStruct UGunComponent::FireShot()
{

	FHitResult LineTraceHit;
	FCollisionQueryParams TraceParams(FName(), false, GetOwner());
	FVector Origin = GetComponentLocation();
	FShotDataStruct Data = FShotDataStruct();
	GetWorld()->LineTraceSingleByChannel(
		OUT LineTraceHit,
		GetComponentLocation(),
		Origin + (GetForwardVector() * ShotRange),
		ECollisionChannel::ECC_GameTraceChannel2,
		TraceParams
	);
	Data.Hit = LineTraceHit.bBlockingHit;
	Data.HitLocation = LineTraceHit.TraceEnd;
	ShotChambered = false;
	if (GunType == EGunType::Automatic) {
		GetWorld()->GetTimerManager().SetTimer(ShotDelayTimer, this, &UGunComponent::TriggerChamberShot, false, ShotDelay);
	}
	GunShotNotify.Broadcast(Data);
	return Data;
}

void UGunComponent::TriggerChamberShot()
{
	AttemptChamberShot();
}

bool UGunComponent::AttemptChamberShot()
{
	if (CurrentAmmo > 0) {
		ShotChambered = true;
		CurrentAmmo -= 1;
		if (GunType == EGunType::Automatic && TriggerDown) {
			FireShot();
		}
	}
	return ShotChambered;
}

void UGunComponent::ShotFired_Implementation(FShotDataStruct ShotData)
{

}



