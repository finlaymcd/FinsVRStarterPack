// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseVRInteractable.h"
#include "Components/StaticMeshComponent.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
#include "Engine.h"
#include "VRPawn.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "PickupObjectComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class VRSTARTERPACK_API UPickupObjectComponent : public UBaseVRInteractable
{
	GENERATED_BODY()
public:
	UPickupObjectComponent();
private:



	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		virtual void CreateDummyMesh();

	UFUNCTION()
		virtual void GrabOn(USceneComponent* Hand, USceneComponent * HandVisual, bool TeleGrab, bool LeftHand) override;

	UFUNCTION()
		virtual void GrabOff(USceneComponent *Hand) override;

	UFUNCTION()
		virtual void OnHover(USceneComponent * Hand, bool Telegrab) override;

	UFUNCTION()
		virtual FVector GetInteractableLocation() override;

	UFUNCTION()
		virtual void TriggerAnimateToHand(USceneComponent* Hand, bool LeftHand);

	UFUNCTION()
		virtual void UpdateAnimateToHand(float Delta);

	UFUNCTION()
		void ResetSoundTime();

	UFUNCTION()
		void CreateTelegrabIndicator();

	UFUNCTION()
		void SetHoverIndicatorVisibilty(bool Visible);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool PhysicsObject = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool AnimateToHandOnGrab = false;

	UPROPERTY(BlueprintReadOnly)
		bool CurrentlyGrabbed = false;

	bool GrabAnimating = false;

	float GrabAnimationLerpFloat = 0.0f;

	USceneComponent * TargetHand = nullptr;
	
	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UStaticMeshComponent * ChildMesh;

	UPROPERTY(Category = Gameplay, VisibleAnywhere)
		UStaticMeshComponent * TelegrabMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
		TArray<USoundCue*> ItemImpactSounds;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
		float ImpactVolume = 1.0f;

	UPROPERTY()
		FTimerHandle ImpactSoundDelayHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
		float ImpactSoundInterval = 0.2f;

	UPROPERTY()
		bool CanPlayImpactSound = true;

	UPROPERTY()
		bool TempHoverIndicatorVisible = false;

	FTransform GrabOffset;

	FVector LeftHandScaleValue;

	FVector RightHandScaleValue;

	UStaticMesh* Asset;

	UStaticMesh * Sphere;


};
