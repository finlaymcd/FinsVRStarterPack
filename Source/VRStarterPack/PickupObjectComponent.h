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



	UFUNCTION()
		virtual void GrabOn(USceneComponent* Hand) override;

	UFUNCTION()
		virtual void GrabOff(USceneComponent *Hand) override;

	UFUNCTION()
		void ResetSoundTime();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool PhysicsObject = false;

	UPROPERTY()
		UStaticMeshComponent * ParentMesh;

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


};
