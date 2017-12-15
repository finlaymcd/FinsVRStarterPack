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
	CreateTelegrabIndicator();
}

void UPickupObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SetHoverIndicatorVisibilty(TempHoverIndicatorVisible);
	TempHoverIndicatorVisible = false;
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

void UPickupObjectComponent::GrabOn(USceneComponent * Hand, bool TeleGrab)
{
	Super::GrabOn(Hand, TeleGrab);

	if (ChildMesh != nullptr) {
		if (PhysicsObject) {
			ChildMesh->SetSimulatePhysics(false);
			ChildMesh->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "None");
		}
		DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		AttachToComponent(Hand, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "None");
		ChildMesh->SetRelativeTransform(GrabOffset, false, nullptr, ETeleportType::None);
	}

}

void UPickupObjectComponent::GrabOff(USceneComponent * Hand)
{
	Super::GrabOff(Hand);
	if (GetAttachmentRoot() != StartingRootComponent && Hand == CurrentInteractingHand && ChildMesh != nullptr) {
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
	TelegrabMesh->SetVisibility(Visible, false);
}

void UPickupObjectComponent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{

	int soundInt = FMath::RandRange(0, ItemImpactSounds.Num() - 1);
	if (ItemImpactSounds[soundInt] != nullptr && CanPlayImpactSound) {
		UGameplayStatics::PlaySoundAtLocation(this, ItemImpactSounds[soundInt], Hit.Location, ChildMesh->GetComponentVelocity().Size() * 0.001f * ImpactVolume);
		CanPlayImpactSound = false;
		GetWorld()->GetTimerManager().SetTimer(ImpactSoundDelayHandle, this, &UPickupObjectComponent::ResetSoundTime, ImpactSoundInterval, false);
	}
	
}
