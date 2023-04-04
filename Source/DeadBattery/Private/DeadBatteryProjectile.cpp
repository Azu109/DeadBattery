// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadBatteryProjectile.h"

ADeadBatteryProjectile::ADeadBatteryProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	// set up a notification for when this component hits something blocking
	
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->bRotationFollowsVelocity = true;

	// Die after 2 seconds by default
	InitialLifeSpan = 2.0f;

	this->SetActorScale3D(FVector(.1, .1, .1));
	
}

void ADeadBatteryProjectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionComp->OnComponentHit.AddDynamic(this, &ADeadBatteryProjectile::OnHit);
}

void ADeadBatteryProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	bool EnemyHit = false;
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor);
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 3.0f, GetActorLocation());
		//UGameplayStatics::SpawnSoundAtLocation(this, GroundHitSFX,this->K2_GetActorLocation(),this->GetActorRotation(),FMath::RandRange(0.7,1.0),FMath::RandRange(0.5,1.5));
	}
	else if (Enemy != nullptr)
	{
		EnemyHit = true;
		//UGameplayStatics::SpawnSoundAtLocation(this, EnemyHitSFX,this->K2_GetActorLocation(),this->GetActorRotation(),FMath::RandRange(0.8,1.2),FMath::RandRange(0.5,1.5));
		Enemy->CurrentHealth -=  ProjectileDamage;
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 2.0f, GetActorLocation());
	}
	/*if(!EnemyHit)
		UGameplayStatics::SpawnSoundAtLocation(this, GroundHitSFX,this->K2_GetActorLocation(),this->GetActorRotation(),.6,FMath::RandRange(0.5,1.5));*/
	
	//UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), spawnEffect, this->GetActorLocation(), Hit.ImpactNormal.Rotation());*/
	Destroy();
}