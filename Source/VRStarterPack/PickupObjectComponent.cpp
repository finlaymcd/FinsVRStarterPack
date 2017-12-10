// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupObjectComponent.h"

UPickupObjectComponent::UPickupObjectComponent()
{

}



void UPickupObjectComponent::BeginPlay()
{
	Super::BeginPlay();


	ParentMesh = Cast<UStaticMeshComponent>(GetAttachParent());
	ParentMesh->OnComponentHit.AddDynamic(this, &UPickupObjectComponent::OnHit);
}

void UPickupObjectComponent::GrabOn(USceneComponent * Hand)
{
	Super::GrabOn(Hand);
	if (PhysicsObject) {
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
			AVRPawn * player = Cast<AVRPawn>(CurrentInteractingHand->GetOwner());
			ParentMesh->SetSimulatePhysics(true);
			ParentMesh->AddForce(ParentMesh->GetComponentVelocity() * (1 + player->PhysicsThrowStrength));
		}
	}
}

void UPickupObjectComponent::ResetSoundTime()
{
	CanPlayImpactSound = true;
}

void UPickupObjectComponent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{

	int soundInt = FMath::RandRange(0, ItemImpactSounds.Num() - 1);
	if (ItemImpactSounds[soundInt] != nullptr && CanPlayImpactSound) {
		UGameplayStatics::PlaySoundAtLocation(this, ItemImpactSounds[soundInt], Hit.Location, ParentMesh->GetComponentVelocity().Size() * 0.001f * ImpactVolume);
		CanPlayImpactSound = false;
		GetWorld()->GetTimerManager().SetTimer(ImpactSoundDelayHandle, this, &UPickupObjectComponent::ResetSoundTime, ImpactSoundInterval, false);
	}
	
}
