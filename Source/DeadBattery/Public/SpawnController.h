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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	void SpawnEnemies();

	UPROPERTY(EditAnywhere, Category = "Spawn Control")
		int32 MinEnemyCount = 0;

	UPROPERTY(EditAnywhere, Category = "Spawn Control")
		int32 MaxEnemyCount = 5;

	UPROPERTY(EditAnywhere, Category = "Spawn Control")
		float TimeBetweenSpawns = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemies", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class AEnemyCharacter> BasicEnemy;

	FTimerHandle SpawnRateTimerHandle;

};
