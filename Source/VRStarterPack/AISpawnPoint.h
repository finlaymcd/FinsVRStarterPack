// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AISpawnPoint.generated.h"

UCLASS()
class VRSTARTERPACK_API AAISpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAISpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void SpawnAI();
	
	UFUNCTION()
		void BeginSpawning();

	UFUNCTION()
		void EndSpawning();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<APawn> AIPawn;
		
	UPROPERTY(BlueprintReadWrite)
		bool CurrentlySpawning = false;

	/*Minimum time between spawns from this spawn point*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MinSpawnInterval = 3.0f;

	/*Minimum time between spawns from this spawn point*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxSpawnInterval = 8.0f;

	/*Stops spawning after this time is elapsed. No limit if left at 0*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SpawnTimeLimit = 0.0f;

	/*Stops spawning after spawning this many AI. No limit if left at 0*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int SpawnLimit = 0;

	FTimerHandle SpawnTimeLimitHandle;
	FTimerHandle SpawnIntervalHandle;
	int NumAISpawned = 0;
};
