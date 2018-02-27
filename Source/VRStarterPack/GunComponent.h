// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine.h"
#include "Components/SceneComponent.h"
#include "GunComponent.generated.h"



USTRUCT(BlueprintType)
struct FShotDataStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot Data Struct")
		bool FiredShot = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot Data Struct")
		bool Hit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot Data Struct")
		FVector HitLocation;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGunShotNotifyDelegate, FShotDataStruct, ShotData);

UENUM(BlueprintType)
enum class EGunType : uint8
{
	Automatic 	UMETA(DisplayName = "Automatic"),
	SemiAutomatic 	UMETA(DisplayName = "Semi Automatic"),
	SingleShot	UMETA(DisplayName = "Single Shot")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRSTARTERPACK_API UGunComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGunComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*Simulates trigger pull. Performs checks that gun can fire before calling FireShot*/
	UFUNCTION(BlueprintCallable)
		void PullTrigger();

	/*Simulates trigger release. Stops firing for automatic, resets firing for semi and single shot*/
	UFUNCTION(BlueprintCallable)
		void ReleaseTrigger();

		/*Traces from the forward vector of the component for targets. Returns data of shot*/
	UFUNCTION(BlueprintCallable)
		FShotDataStruct FireShot();

	UFUNCTION()
		void TriggerChamberShot();

	UFUNCTION(BlueprintCallable)
		bool AttemptChamberShot();

	UFUNCTION(BlueprintNativeEvent)
		void ShotFired(FShotDataStruct ShotData);

	/*How does the gun fire?*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EGunType GunType = EGunType::SingleShot;

	/*How far is traced for shot*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ShotRange = 500.0f;

	/*Damage per shot is randomised between the min and max damage*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MinDamage = 10.0f;

	/*Damage per shot is randomised between the min and max damage*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxDamage = 12.0f;

	/*Largest number of shots that can be held in the weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MaxAmmo = 10;

	/*Largest number of shots that can be held in the weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int CurrentAmmo = 10;

	/*Mostly to be changed for shotguns. The amount of traces to do per shot in a single frame*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ShotsPerTriggerPull = 1;

	/*Minimum time between shots*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ShotDelay = 0.4f;

	/*Gun will only fire when this is true*/
	UPROPERTY(BlueprintReadWrite)
		bool ShotChambered = true;

	UPROPERTY(BlueprintReadWrite)
		bool TriggerDown = false;

	UPROPERTY(BlueprintAssignable, Category = Grabbing)
		FGunShotNotifyDelegate GunShotNotify;

	FTimerHandle ShotDelayTimer;


	
};
