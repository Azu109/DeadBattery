// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 *
 */
UCLASS()
class DEADBATTERY_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	AEnemyAIController();
	int EnemyType;
	bool CanFire;
	float FireRate;
	float FireRateTimer;
	
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ADeadBatteryProjectile> CannonProjectile;
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void SetEnemyType(int Type);

	int GetEnemyType();

};