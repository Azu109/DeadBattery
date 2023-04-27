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

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsFlinching;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool InRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DeathAnimDuration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRateTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FirstTimeShotTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FlinchAnimDuration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FlinchTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnemyType EnemyType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MeleeAttackDuration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MeleeAttackTimer;
    	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsMeleeAttacking;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceToPlayerBeforeShooting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaximumDistanceToShoot;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* DeathParticleEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ADeadBatteryProjectile> ShootingEnemyProjectile;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
    class USoundBase* DeathExplosionSFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundBase* EnemyCannonSFX;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(VisibleDefaultsOnly)
	UCapsuleComponent* CollisionCompCap;
    
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UCapsuleComponent* GetCollisionComp() const { return CollisionCompCap; }

};
