// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupObjectComponent.h"

UPickupObjectComponent::UPickupObjectComponent()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	Asset = MeshAsset.Object;
}



void UPickupObjectComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<USceneComponent*> Meshes;
	GetChildrenComponents(false, Meshes);
	if (Meshes.Num() > 0) {
		UE_LOG(LogTemp, Warning, TEXT("Found %d static meshes"), Meshes.Num());
		for (int i = 0; i < Meshes.Num(); i++) {
			if (Meshes[i]->IsA(UStaticMeshComponent::StaticClass())) {
				ParentMesh = Cast<UStaticMeshComponent>(Meshes[i]);
			}
		}
	}
	//ParentMesh = Cast<UStaticMeshComponent>(GetAttachParent());
	if (ParentMesh != nullptr) {
		ParentMesh->OnComponentHit.AddDynamic(this, &UPickupObjectComponent::OnHit);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Pickup Object Component Warning on %s: No Static Mesh Component found in children of Pickup Object Component. Creating dummy mesh."), *GetOwner()->GetName());
		CreateDummyMesh();
	}

	GrabOffset = ParentMesh->GetRelativeTransform();
}

void UPickupObjectComponent::CreateDummyMesh()
{
	FName ObjectName("Dummy Mesh");
	UStaticMeshComponent* NewComp = NewObject<UStaticMeshComponent>(this, ObjectName);
	if (!NewComp)
	{
		return;
	}

	NewComp->RegisterComponent();        
	NewComp->SetWorldLocation(GetComponentLocation());
	NewComp->SetWorldRotation(GetComponentRotation());
	NewComp->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform, "None");
	ParentMesh = NewComp;

	ParentMesh->SetStaticMesh(Asset);

}

void UPickupObjectComponent::GrabOn(USceneComponent * Hand, bool TeleGrab)
{
	Super::GrabOn(Hand, TeleGrab);

	if (ParentMesh != nullptr) {
		if (PhysicsObject) {
			ParentMesh->SetSimulatePhysics(false);
			ParentMesh->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "None");
		}
		DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		AttachToComponent(Hand, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "None");
		ParentMesh->SetRelativeTransform(GrabOffset, false, nullptr, ETeleportType::None);
	}

}

void UPickupObjectComponent::GrabOff(USceneComponent * Hand)
{
	Super::GrabOff(Hand);
	if (GetAttachmentRoot() != StartingRootComponent && Hand == CurrentInteractingHand && ParentMesh != nullptr) {
		//GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		AttachToComponent(StartingRootComponent, FAttachmentTransformRules::KeepWorldTransform);
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
