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

	NumOfSpawnPointsStart = SpawnPoints.Num() / 2;

	GetWorldTimerManager().SetTimer(SpawnRateTimerHandle, this, &ASpawnController::SpawnEnemies, TimeBetweenSpawns, true);		// Call Spawn Enemies every X amount of Seconds

	EnemiesToSpawn.Add(BasicEnemy);
	EnemiesToSpawn.Add(GhostEnemy);
	
}

// Called every frame
void ASpawnController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), EnemiesInLevel);					// Get All Enemies in Level

	if (EnemiesInLevel.Num() <= 0) {																					// Check if no Enemies in Level
		SpawnEnemies();
		if (GetWorldTimerManager().IsTimerActive(SpawnRateTimerHandle))
			GetWorldTimerManager().ClearTimer(SpawnRateTimerHandle);
		GetWorldTimerManager().SetTimer(SpawnRateTimerHandle, this, &ASpawnController::SpawnEnemies, TimeBetweenSpawns, true);
	}

}


// Spawn Enemies 
void ASpawnController::SpawnEnemies()
{
	UE_LOG(LogTemp, Warning, TEXT("Spawn Enemies"));	

	// SPAWN BASED ON SPAWN POINTS
	/*
	for (AActor* Actor : SpawnPoints) 
	{
		int Quantity = FMath::RandRange(MinEnemyCount, MaxEnemyCount);				// Random Number of Enemies to Spawn

		for (int i = 0; i < Quantity; i++) {
			GetWorld()->SpawnActor<AEnemyCharacter>(BasicEnemy, Actor->GetActorLocation(), Actor->GetActorRotation());				
		}
	}
	*/



	// SPAWN BASED ON TOTAL ENEMIES IN LEVEL

	Wave++;
	UE_LOG(LogTemp, Warning, TEXT("Spawning No.: %d"), NumOfEnemies);

	// Randomly spawn between spawn points
	for (int i = 1; i <= NumOfEnemies; i++) {
		int RandomSpawner = FMath::RandRange(0, SpawnPoints.Num()-1);
		int RandomEnemy = FMath::RandRange(0, EnemiesToSpawn.Num()-1);
		AEnemyCharacter* Enemy =  GetWorld()->SpawnActor<AEnemyCharacter>(EnemiesToSpawn[RandomEnemy], SpawnPoints[RandomSpawner]->GetActorLocation(), SpawnPoints[RandomSpawner]->GetActorRotation());
		if(Enemy != nullptr)
		{
			Enemy->MaxHealth += 10 * (NumOfEnemies-1); 
			Enemy->CurrentHealth += 10 * (NumOfEnemies-1);
		} 
	}
	EnemyCountInWave();
	TArray<AActor*> CurrentEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), CurrentEnemies);
}


// Increase Num of Enemies per wave
void ASpawnController::EnemyCountInWave() {
	//NumOfEnemies = NumOfEnemies + (NumOfEnemies / 2);
	NumOfEnemies ++;
}

