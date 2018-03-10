// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "AISpawnPoint.h"
#include "VRPawn.h"
#include "AISpawnTrigger.generated.h"

UENUM(BlueprintType)
enum class ETriggerType : uint8
{
	ActivateOnEntrance 		UMETA(DisplayName = "Activate On Entrance"),
	DeActivateOnEntrance 	UMETA(DisplayName = "Deactivate On Entrance"),
	ActiveWhileInVolume		UMETA(DisplayName = "Active While In Volumue")
};

UCLASS()
class VRSTARTERPACK_API AAISpawnTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAISpawnTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OverlapTriggered(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void EndOverlapTriggered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
		void PlayerEnteredArea(AActor * Player);


	UPROPERTY(EditDefaultsOnly)
		UBoxComponent * TriggerBox;

	UPROPERTY(EditAnywhere)
		TArray<AAISpawnPoint*> SpawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETriggerType TriggerSpawnType = ETriggerType::ActivateOnEntrance;


};
