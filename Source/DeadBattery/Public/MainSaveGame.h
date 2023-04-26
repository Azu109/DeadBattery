// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MainSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class DEADBATTERY_API UMainSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UMainSaveGame();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double HighScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double LastScore;
	
};
