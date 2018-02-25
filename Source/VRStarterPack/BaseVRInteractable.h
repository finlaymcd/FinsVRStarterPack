// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "BaseVRInteractable.generated.h"

class UInteractableHandComponent;

/*For selecting what type of input to use*/
UENUM(BlueprintType)		
enum class EInteractButtonEnum : uint8
{
	Trigger 	UMETA(DisplayName = "Trigger"),
	ButtonOne 	UMETA(DisplayName = "Button One"),
	ButtonTwo	UMETA(DisplayName = "Button Two")
};

/*For filtering out interaction event to blueprint*/
UENUM(BlueprintType)
enum class EInteractType : uint8
{
	Grab		UMETA(DisplayName = "Grab"),
	InteractOne	UMETA(DisplayName = "InteractOne"),
	InteractTwo	UMETA(DisplayName = "InteractTwo"),
	InteractThree UMETA(DisplayName = "InteractThree")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionDelegate, EInteractType, InteractionType, bool, InteractionDown);

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

	UFUNCTION()
		virtual void GrabOn(USceneComponent* Hand, USceneComponent * HandVisual, UInteractableHandComponent * HandLogic, bool TeleGrab, bool LeftHand); //Grabbed using grip buttons

	UFUNCTION()
		virtual void GrabOff(USceneComponent *Hand); //Released grip buttons

	UFUNCTION()
		virtual void InteractOne(USceneComponent *Hand, float Value); //Interaction while holding

	UFUNCTION()
		virtual void InteractTwo(USceneComponent* Hand, float Value); //Interaction while holding

	UFUNCTION()
		virtual void InteractThree(USceneComponent* Hand, float Value); //Interaction while holding

	UFUNCTION()
		virtual void DualAxisInteractUpdate(USceneComponent * Hand, float XValue, float YValue); //Interaction while holding

	UFUNCTION()
		virtual void OnHover(USceneComponent * Hand, bool Telegrab);

	UFUNCTION()
		virtual FVector GetInteractableLocation();

	UFUNCTION(BlueprintNativeEvent, Category = Grabbing)
		void NotifyAttemptedInteraction(EInteractType Interaction, bool InteractionDown);

	UPROPERTY(BlueprintReadOnly)
		bool GrabDown = false;

	UPROPERTY(BlueprintReadOnly)
		bool InteractOneDown = false;

	UPROPERTY(BlueprintReadOnly)
		bool InteractTwoDown = false;

	UPROPERTY(BlueprintReadOnly)
		bool InteractThreeDown = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool CanTeleGrab = true;

	UPROPERTY(EditAnywhere)
		EInteractButtonEnum InteractButtonOne = EInteractButtonEnum::Trigger;

	UPROPERTY(EditAnywhere)
		EInteractButtonEnum InteractButtonTwo = EInteractButtonEnum::ButtonOne;

	UPROPERTY(EditAnywhere)
		EInteractButtonEnum InteractButtonThree = EInteractButtonEnum::ButtonTwo;

	UPROPERTY(BlueprintAssignable, Category = Grabbing)
		FInteractionDelegate InteractionNotification;

	UPROPERTY(BlueprintReadOnly)
		USceneComponent * CurrentInteractingHand;

	UPROPERTY(BlueprintReadOnly)
		USceneComponent * CurrentHandVisual;

	UPROPERTY(BlueprintReadWrite)
		UInteractableHandComponent * CurrentHandLogic;

	UPROPERTY(BlueprintReadWrite)
		USceneComponent * StartingRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AnimGrabValue = 0.5f;



	

};
