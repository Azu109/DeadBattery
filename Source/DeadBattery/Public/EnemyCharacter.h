// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UENUM(BlueprintType)
enum class EEnemyType : uint8 {
	ET_Shooting       UMETA(DisplayName="Shooting Enemy"),
	ET_Melee        UMETA(DisplayName="Melee Enemy"),
};

UCLASS()
class DEADBATTERY_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth;
	
	float CurrentHealth;

	bool CanFire;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate;
	
	float FireRateTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnemyType EnemyType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceToPlayerBeforeShooting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ADeadBatteryProjectile> ShootingEnemyProjectile;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
