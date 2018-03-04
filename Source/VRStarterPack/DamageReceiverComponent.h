// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageReceiverComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FDamageNotification, float, DamageAmount, AActor *, DamageActor, USceneComponent *, DamageComponent, FVector, DamageLocation);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRSTARTERPACK_API UDamageReceiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDamageReceiverComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual void TakeDamage(float DamageAmount, AActor * DamageActor, USceneComponent * DamageComponent, FVector DamageLocation);

	UFUNCTION(BlueprintCallable)
	virtual void HealthDepleted(float DamageAmount, AActor * DamageActor, USceneComponent * DamageComponent, FVector DamageLocation);

	UFUNCTION(BlueprintNativeEvent)
	void DamageTaken(float DamageAmount, AActor * DamageActor, USceneComponent * DamageComponent, FVector DamageLocation);

	UFUNCTION(BlueprintNativeEvent)
	void HealthDepletedNotification(float DamageAmount, AActor * DamageActor, USceneComponent * DamageComponent, FVector DamageLocation);

	/*If this is true, when the object receives damage it will lose health and potentially call the health depleated function*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool LosesHealthOnDamage = true;

	/*Largest amount of health object can have*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditCondition = "LosesHealthOnDamage"))
		float MaxHealth = 100.0f;

	/*Starting health. If it falls below 0.0f the health depleted function is called*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditCondition = "LosesHealthOnDamage"))
		float CurrentHealth = 100.0f;
	
	UPROPERTY(BlueprintAssignable)
		FDamageNotification DamageNotification;

	UPROPERTY(BlueprintAssignable)
		FDamageNotification HealthDepletedDelegate;

	UPROPERTY()
		bool Depleted = false;
};
