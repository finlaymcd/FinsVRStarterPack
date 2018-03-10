// Fill out your copyright notice in the Description page of Project Settings.

#include "AISpawnPoint.h"


// Sets default values
AAISpawnPoint::AAISpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAISpawnPoint::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAISpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAISpawnPoint::SpawnAI()
{
	if (CurrentlySpawning) {
		if (AIPawn != nullptr) {
			FVector Location = GetActorLocation();
			FRotator Rotation = GetActorRotation();
			FActorSpawnParameters SpawnInfo;
			APawn * AI = GetWorld()->SpawnActor<APawn>(AIPawn, Location, Rotation, SpawnInfo);
			AI->SpawnDefaultController();
			NumAISpawned++;
		}
		if (SpawnLimit == 0 || NumAISpawned < SpawnLimit) {
			float Interval = FMath::RandRange(MinSpawnInterval, MaxSpawnInterval);
			GetWorld()->GetTimerManager().SetTimer(SpawnIntervalHandle, this, &AAISpawnPoint::SpawnAI, Interval, false);
		}
		else {
			EndSpawning();
		}
	}
}

void AAISpawnPoint::BeginSpawning()
{
	CurrentlySpawning = true;
	if (SpawnTimeLimit != 0.0f) {
		GetWorld()->GetTimerManager().SetTimer(SpawnTimeLimitHandle, this, &AAISpawnPoint::EndSpawning, SpawnTimeLimit, false);
	}
	float Interval = FMath::RandRange(MinSpawnInterval, MaxSpawnInterval);
	GetWorld()->GetTimerManager().SetTimer(SpawnIntervalHandle, this, &AAISpawnPoint::SpawnAI, Interval, false);
}

void AAISpawnPoint::EndSpawning()
{
	CurrentlySpawning = false;
}

