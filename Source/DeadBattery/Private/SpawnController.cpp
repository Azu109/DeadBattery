// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnController.h"
#include "kismet/GameplayStatics.h"
#include "EnemyCharacter.h"

// Sets default values
ASpawnController::ASpawnController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawnController::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), SpawnPoints);								// Get All Spawn Points in World
	UE_LOG(LogTemp, Warning, TEXT("Array Count: %d"), SpawnPoints.Num());

	GetWorldTimerManager().SetTimer(SpawnRateTimerHandle, this, &ASpawnController::SpawnEnemies, TimeBetweenSpawns, true);		// Call Spawn Enemies every X amount of Seconds
	
}

// Called every frame
void ASpawnController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Spawn Enemies 
void ASpawnController::SpawnEnemies()
{
	UE_LOG(LogTemp, Warning, TEXT("Spawn Enemies"));							
	for (AActor* Actor : SpawnPoints) 
	{
		int Quantity = FMath::RandRange(MinEnemyCount, MaxEnemyCount);				// Random Number of Enemies to Spawn

		for (int i = 0; i < Quantity; i++) {
			GetWorld()->SpawnActor<AEnemyCharacter>(BasicEnemy, Actor->GetActorLocation(), Actor->GetActorRotation());				
		}
	}
}

