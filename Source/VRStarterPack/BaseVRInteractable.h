// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BaseVRInteractable.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EInteractButtonEnum : uint8
{
	Trigger 	UMETA(DisplayName = "Trigger"),
	ButtonOne 	UMETA(DisplayName = "Button One"),
	ButtonTwo	UMETA(DisplayName = "Button Two")
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRSTARTERPACK_API UBaseVRInteractable : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseVRInteractable();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GrabOn(USceneComponent Hand); //Grabbed using grip buttons

	virtual void GrabOff(USceneComponent Hand); //Released grip buttons

	virtual void InteractOn(USceneComponent Hand); //Interaction while holding

	virtual void InteractOff(USceneComponent Hand); //release interaction while holding

	UPROPERTY(VisibleAnywhere)
		bool GrabDown = false;

	UPROPERTY(VisibleAnywhere)
		bool InteractDown = false;
	
	UPROPERTY(EditAnywhere)
		EInteractButtonEnum InteractButton = EInteractButtonEnum::Trigger;
};
