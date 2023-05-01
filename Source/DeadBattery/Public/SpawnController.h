// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TargetPoint.h"
#include "SpawnController.generated.h"

UCLASS()
class DEADBATTERY_API ASpawnController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnController();

	UPROPERTY(EditAnywhere)
		TArray<AActor*> SpawnPoints;

	UPROPERTY(EditAnywhere)
		TArray<AActor*> EnemiesInLevel;

	UPROPERTY(EditAnywhere)
		TArray <TSubclassOf<class AEnemyCharacter>> EnemiesToSpawn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemies")
	TSubclassOf<class AEnemyCharacter> BasicEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemies")
	TSubclassOf<class AEnemyCharacter> GhostEnemy;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Spawn Control")
		int Wave = 0;
	
private:
	void SpawnEnemies();

	UPROPERTY(EditAnywhere, Category = "Spawn Control")
		int MinEnemyCount = 0;

	UPROPERTY(EditAnywhere, Category = "Spawn Control")
		int MaxEnemyCount = 5;

	UPROPERTY(EditAnywhere, Category = "Spawn Control")
		float TimeBetweenSpawns = 10;

	UPROPERTY(EditAnywhere, Category = "Spawn Control")
		int NumOfSpawnPointsStart = 3;

	UPROPERTY(EditAnywhere, Category = "Spawn Control")
		int NumOfEnemies = 5;

	

	

	FTimerHandle SpawnRateTimerHandle;


	void EnemyCountInWave();

};
