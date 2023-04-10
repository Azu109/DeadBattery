// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.generated.h"

/**
 *
 */



UCLASS()
class DEADBATTERY_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

	
	
	AEnemyAIController();
	


	
	
public:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void Shoot(AEnemyCharacter* EnemyCharacter);
	
};