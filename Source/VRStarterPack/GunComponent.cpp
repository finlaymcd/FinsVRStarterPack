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

TArray<FShotDataStruct> UGunComponent::FireShot()
{

	FHitResult LineTraceHit;
	FCollisionQueryParams TraceParams(FName(), false, GetOwner());
	FVector Origin = GetComponentLocation();
	FRotator Forward = GetForwardVector().Rotation();
	TArray<FShotDataStruct> Data;
	for (int i = 0; i < ShotsPerTriggerPull; i++) {
		FShotDataStruct D = FShotDataStruct();
		Forward.Yaw += FMath::RandRange(MinBulletSpread, MaxBulletSpread);
		Forward.Pitch += FMath::RandRange(MinBulletSpread, MaxBulletSpread);
		FVector FinalVector = UKismetMathLibrary::GetForwardVector(Forward);
		GetWorld()->LineTraceSingleByChannel(
			OUT LineTraceHit,
			GetComponentLocation(),
			Origin + (FinalVector * ShotRange),
			ECollisionChannel::ECC_GameTraceChannel2,
			TraceParams
		);
		
		D.FiredShot = true;
		D.Hit = LineTraceHit.bBlockingHit;
		D.HitLocation = LineTraceHit.Location;
		if (!D.Hit) {
			D.HitLocation = LineTraceHit.TraceEnd;
		}
		else {
			TArray<UDamageReceiverComponent*> DamageReceiver;
			LineTraceHit.Actor->GetComponents<UDamageReceiverComponent>(DamageReceiver);
			if (DamageReceiver.IsValidIndex(0)) {
				float Damage = FMath::RandRange(MinDamage, MaxDamage);
				AActor * actor = GetOwner();
				DamageReceiver[0]->TakeDamage(Damage, actor, D.HitLocation);
			}
		}
		Data.Add(D);

	}
//	Data.Hit = LineTraceHit.bBlockingHit;
	//Data.HitLocation = LineTraceHit.TraceEnd;
	ShotChambered = false;
	if (GunType == EGunType::Automatic) {
		GetWorld()->GetTimerManager().SetTimer(ShotDelayTimer, this, &UGunComponent::TriggerChamberShot, ShotDelay, false);
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

bool UGunComponent::InsertAmmo(int Amount)
{
	if (Amount + CurrentAmmo > MaxAmmo) {
		return false;
	}
	else {
		CurrentAmmo += Amount;
		return true;
	}
}

void UGunComponent::ShotFired_Implementation(const TArray<FShotDataStruct>& ShotData)
{

}



