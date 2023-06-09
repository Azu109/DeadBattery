// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "Components/CapsuleComponent.h"
#include "DeadBattery/DeadBatteryCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionCompCap = GetCapsuleComponent();
	CollisionCompCap->SetCapsuleRadius(50.0f);
	IsDead = false;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	CanFire = false;
	//FireRate = 1;
	FireRateTimer = AnimDuration;
	IsFlinching = false;
	
	IsMeleeAttacking = false;
	MeleeAttackTimer = MeleeAttackDuration;

	FlinchTimer = FlinchAnimDuration;
	
	CollisionCompCap->OnComponentHit.AddDynamic(this, &AEnemyCharacter::OnHit);
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(CurrentHealth<=0)
	{
		IsDead = true;
	}

	if(IsDead)
		DeathAnimDuration -= DeltaTime;

	if(IsDead && DeathAnimDuration <=0)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, DeathExplosionSFX,this->K2_GetActorLocation());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticleEffect, GetTransform());
		Destroy();
	}

	if(!CanFire)
	{
		FireRateTimer -= DeltaTime;
		if(FireRateTimer<=0)
		{
			FireRateTimer = AnimDuration;
			CanFire = true;
		}
	}

	if(IsFlinching)
	{
		FlinchTimer -= DeltaTime;
		if(FlinchTimer<=0)
		{
			FlinchTimer = FlinchAnimDuration;
			IsFlinching = false;
		}
	}

	if(IsMeleeAttacking)
	{
		MeleeAttackTimer -= DeltaTime;
		if(MeleeAttackTimer<=0)
		{
			MeleeAttackTimer = MeleeAttackDuration;
			IsMeleeAttacking = false;
		}
	}
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyCharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ADeadBatteryCharacter* Player = Cast<ADeadBatteryCharacter>(OtherActor);
	// Only add impulse and destroy projectile if we hit a physics
	if (Player != nullptr && EnemyType == EEnemyType::ET_Melee)
	{
		//Player->BloodMeterChange(-0.10);
		IsMeleeAttacking = true;
	}
}

