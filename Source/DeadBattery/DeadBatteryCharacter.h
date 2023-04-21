// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "DeadBatteryCharacter.generated.h"


UCLASS(config=Game)
class ADeadBatteryCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	
	/** Shoot Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction* ShootAction;
    
    /** Aim Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction* AimAction;

	/** Shield Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ShieldAction;

	
public:
	ADeadBatteryCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ADeadBatteryProjectile> CannonProjectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AActor> PlayerShield;

	AActor* shield;

	bool ShieldSpawned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	float MaxBloodMeter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	float MaxEnergyMeter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health)
	float CurrentBloodMeter;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health)
	float CurrentEnergyMeter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float FireRate; // In RPM
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float EnergyDrainPerShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float EnergyDrainRate; // Unit per second

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float BloodDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float BloodMeterGainOnKill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float ShieldDrainRate;
	

	float FireRateTimer;

	float DeltaTime;

	bool CanFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	double Score;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	double Timer;
	
	FRotator AimRotation;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class USkeletalMeshComponent* Mesh;*/

	FVector LaunchDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attacking)
	bool IsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attacking)
	bool IsMeleeAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attacking)
	float MeleeCooldownDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attacking)
	float MeleeCooldownTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	bool IsSprinting;


	//Audio

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundBase* CannonFireSFX;

	
	UFUNCTION()
    	void BloodMeterChange(float Change);
    
    	void EnergyMeterChange(float Change);


protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for Sprint input */
	void StartSprinting(const FInputActionValue& Value);

	/** Called for Sprint input */
	void StopSprinting(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for shooting input */
	void Shoot(const FInputActionValue& Value);
	
	/** Called for aiming input */ 
    void Aim(const FInputActionValue& Value);

	/** Called for stop aiming input */ 
	void StopAiming(const FInputActionValue& Value);

	/** Called for Shield input */
	void StartShield(const FInputActionValue& Value);
	
	/** Called for Shield input */
	void StopShield(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// BEGIN PLAY
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(VisibleDefaultsOnly)
	UCapsuleComponent* CollisionCompCap;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* HealParticleEffect;
    
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UCapsuleComponent* GetCollisionComp() const { return CollisionCompCap; }
	
};


