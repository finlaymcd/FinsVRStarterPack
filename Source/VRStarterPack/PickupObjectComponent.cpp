// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupObjectComponent.h"

UPickupObjectComponent::UPickupObjectComponent()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	Asset = MeshAsset.Object;
	Sphere = SphereAsset.Object;
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
				ChildMesh = Cast<UStaticMeshComponent>(Meshes[i]);
			}
		}
	}
	//ParentMesh = Cast<UStaticMeshComponent>(GetAttachParent());
	if (ChildMesh != nullptr) {
		ChildMesh->OnComponentHit.AddDynamic(this, &UPickupObjectComponent::OnHit);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Pickup Object Component Warning on %s: No Static Mesh Component found in children of Pickup Object Component. Creating dummy mesh."), *GetOwner()->GetName());
		CreateDummyMesh();
	}

	GrabOffset = ChildMesh->GetRelativeTransform();
	RightHandScaleValue = GetComponentScale();
	LeftHandScaleValue = FVector(RightHandScaleValue.X, RightHandScaleValue.Y * -1, RightHandScaleValue.Z);
	CreateTelegrabIndicator();
}

void UPickupObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SetHoverIndicatorVisibilty(TempHoverIndicatorVisible);
	TempHoverIndicatorVisible = false;
	if (GrabAnimating) {
		UpdateAnimateToHand(DeltaTime);
	}
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
	ChildMesh = NewComp;

	ChildMesh->SetStaticMesh(Asset);

}

void UPickupObjectComponent::GrabOn(USceneComponent * Hand, USceneComponent * HandVisual, UInteractableHandComponent * HandLogic, bool TeleGrab, bool LeftHand)
{
	Super::GrabOn(Hand, HandVisual, HandLogic, TeleGrab, LeftHand);

	if (ChildMesh != nullptr) {
		CurrentlyGrabbed = true;
		if (AnimateToHandOnGrab) {
			TriggerAnimateToHand(Hand, LeftHand);
		}
		else {
			if (PhysicsObject) {
				ChildMesh->SetSimulatePhysics(false);
				ChildMesh->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "None");
			}
			DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			AttachToComponent(Hand, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "None");
			ChildMesh->SetRelativeTransform(GrabOffset, false, nullptr, ETeleportType::None);
			if (LeftHand) {
				//ChildMesh->SetRelativeLocation(FVector(ChildMesh->RelativeLocation.X, ChildMesh->RelativeLocation.Y * -1, ChildMesh->RelativeLocation.Z));
				//SetRelativeScale3D(LeftHandScaleValue);
				SetWorldScale3D(LeftHandScaleValue);
			}
			else {
				//SetRelativeScale3D(RightHandScaleValue);
				SetWorldScale3D(LeftHandScaleValue);
			}
		}
	}

}

void UPickupObjectComponent::GrabOff(USceneComponent * Hand)
{
	Super::GrabOff(Hand);
	if (GetAttachmentRoot() != StartingRootComponent && Hand == CurrentInteractingHand && ChildMesh != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("GRAB OFF"));
		CurrentlyGrabbed = false;
		GrabAnimating = false;
		//GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		AttachToComponent(StartingRootComponent, FAttachmentTransformRules::KeepWorldTransform);
		if (PhysicsObject) {
			AVRPawn * player = Cast<AVRPawn>(CurrentInteractingHand->GetOwner());
			ChildMesh->SetSimulatePhysics(true);
			ChildMesh->AddForce(ChildMesh->GetComponentVelocity() * (1 + player->PhysicsThrowStrength));
		}
	}
}

void UPickupObjectComponent::OnHover(USceneComponent * Hand, bool Telegrab)
{
	TempHoverIndicatorVisible = true;
}

FVector UPickupObjectComponent::GetInteractableLocation()
{
	return ChildMesh->GetComponentLocation();
}

void UPickupObjectComponent::TriggerAnimateToHand(USceneComponent * Hand, bool LeftHand)
{
	TargetHand = Hand;
	GrabAnimating = true;
	GrabAnimationLerpFloat = 0.0f;
	FVector pos = ChildMesh->GetComponentLocation();
	if (PhysicsObject) {
		ChildMesh->SetSimulatePhysics(false);
		ChildMesh->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "None");
	}
	DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	AttachToComponent(Hand, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "None");
	if (LeftHand) {
		//ChildMesh->SetRelativeLocation(FVector(ChildMesh->RelativeLocation.X, ChildMesh->RelativeLocation.Y * -1, ChildMesh->RelativeLocation.Z));
		SetRelativeScale3D(LeftHandScaleValue);
	}
	else {
		SetRelativeScale3D(RightHandScaleValue);
	}
	ChildMesh->SetWorldLocation(pos);



}

void UPickupObjectComponent::UpdateAnimateToHand(float Delta)
{
	if (GrabAnimationLerpFloat > 1) {
		GrabAnimationLerpFloat = 1.0f;
		GrabAnimating = false;
	}
	FVector Current = ChildMesh->RelativeLocation;
	FVector Target = GrabOffset.GetLocation();
	FRotator CurrentRot = ChildMesh->RelativeRotation;
	FRotator TargetRot = GrabOffset.Rotator();

	GrabAnimationLerpFloat += Delta;

	FVector NewLocation = FMath::Lerp(Current, Target, GrabAnimationLerpFloat);
	FRotator NewRotation = FMath::Lerp(CurrentRot, TargetRot, GrabAnimationLerpFloat);
	ChildMesh->SetRelativeLocation(NewLocation);
	ChildMesh->SetRelativeRotation(NewRotation);

}

void UPickupObjectComponent::ResetSoundTime()
{
	CanPlayImpactSound = true;
}

void UPickupObjectComponent::CreateTelegrabIndicator()
{
	FName ObjectName("Telegrab Indicator");
	UStaticMeshComponent* NewComp = NewObject<UStaticMeshComponent>(this, ObjectName);
	if (!NewComp)
	{
		return;
	}

	NewComp->RegisterComponent();
	NewComp->SetWorldLocation(ChildMesh->GetComponentLocation());
	NewComp->SetWorldRotation(ChildMesh->GetComponentRotation());
	NewComp->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	NewComp->AttachToComponent(ChildMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "None");

	TelegrabMesh = NewComp;

	TelegrabMesh->SetStaticMesh(Sphere);
}

void UPickupObjectComponent::SetHoverIndicatorVisibilty(bool Visible)
{
	if (CurrentlyGrabbed) {
		Visible = false;
	}
		TelegrabMesh->SetVisibility(Visible, false);	
}

void UPickupObjectComponent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ItemImpactSounds.Num() > 0) {
		int soundInt = FMath::RandRange(0, ItemImpactSounds.Num() - 1);
		if (ItemImpactSounds[soundInt] != nullptr && CanPlayImpactSound) {
			UGameplayStatics::PlaySoundAtLocation(this, ItemImpactSounds[soundInt], Hit.Location, ChildMesh->GetComponentVelocity().Size() * 0.001f * ImpactVolume);
			CanPlayImpactSound = false;
			GetWorld()->GetTimerManager().SetTimer(ImpactSoundDelayHandle, this, &UPickupObjectComponent::ResetSoundTime, ImpactSoundInterval, false);
		}
	}
	
}
