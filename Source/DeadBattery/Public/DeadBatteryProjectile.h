// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DeadBatteryProjectile.generated.h"


/**
 * 
 */
UCLASS()
class DEADBATTERY_API ADeadBatteryProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;


	/*UPROPERTY(EditAnywhere, Category = ProjectileEffects)
	UNiagaraSystem* spawnEffect;*/

	

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	FTimerHandle MemberTimerHandle;

public:
	ADeadBatteryProjectile();
	/** called when projectile hits something */

	//Begin Play
	virtual void BeginPlay();
	
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Particle effects will later be required
	
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PE)
	class USoundBase* EnemyHitSFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PE)
	class USoundBase* GroundHitSFX;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float ProjectileDamage;
	
	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	
};
