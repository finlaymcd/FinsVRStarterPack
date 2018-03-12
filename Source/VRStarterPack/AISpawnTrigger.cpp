// Fill out your copyright notice in the Description page of Project Settings.

#include "AISpawnTrigger.h"


// Sets default values
AAISpawnTrigger::AAISpawnTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(GetRootComponent());
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AAISpawnTrigger::OverlapTriggered);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AAISpawnTrigger::EndOverlapTriggered);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void AAISpawnTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAISpawnTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAISpawnTrigger::OverlapTriggered(UPrimitiveComponent * OverlappingComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(Cast<AVRPawn>(OtherActor)) {
		PlayerEnteredArea(OtherActor);
	}
}

void AAISpawnTrigger::EndOverlapTriggered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (TriggerSpawnType == ETriggerType::ActiveWhileInVolume && Cast<AVRPawn>(OtherActor)) {
		for (int i = 0; i < SpawnPoints.Num(); i++) {
			SpawnPoints[i]->EndSpawning();
		}
	}
}

void AAISpawnTrigger::PlayerEnteredArea(AActor * Player)
{
	if (TriggerSpawnType == ETriggerType::ActivateOnEntrance || TriggerSpawnType == ETriggerType::ActiveWhileInVolume) {
		for (int i = 0; i < SpawnPoints.Num(); i++) {
			if (SpawnPoints[i] != nullptr) {
				SpawnPoints[i]->BeginSpawning();
			}
		}
	}
	else {
		for (int i = 0; i < SpawnPoints.Num(); i++) {
			if (SpawnPoints[i] != nullptr) {
			SpawnPoints[i]->EndSpawning();
			}
		}
	}
}

