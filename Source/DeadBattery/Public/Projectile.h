// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"


class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config = Game)
class AProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(EditAnywhere, Category = Projectile)
	int ProjectileDamage;

	/*UPROPERTY(EditAnywhere, Category = ProjectileEffects)
	UNiagaraSystem* spawnEffect;*/

	

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	FTimerHandle MemberTimerHandle;

public:
	AProjectile();
	/** called when projectile hits something */
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Particle effects will later be required
	
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PE)
	class USoundBase* EnemyHitSFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PE)
	class USoundBase* GroundHitSFX;*/

	
	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};