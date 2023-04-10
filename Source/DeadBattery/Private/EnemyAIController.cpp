// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "DeadBatteryProjectile.h"
#include "EnemyCharacter.h"
#include "VectorTypes.h"
#include "DeadBattery/DeadBatteryCharacter.h"
#include "kismet/GameplayStatics.h"

AEnemyAIController::AEnemyAIController()
{
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	CanFire = false;
	//FireRate = 1;
	FireRateTimer = 1.0f / (FireRate/60.0f);
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(!CanFire)
	{
		FireRateTimer -= DeltaSeconds;
		if(FireRateTimer<=0)
		{
			FireRateTimer = 1.0f / (FireRate/60.0f);
			CanFire = true;
		}
	}

	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn());						// Get Enemy Character it is Controlling

	if (EnemyCharacter != NULL) {
		ADeadBatteryCharacter* PlayerCharacter = Cast<ADeadBatteryCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));		// Get Player Character

		if (PlayerCharacter != NULL) {
			if (EnemyType == EEnemyType::ET_Melee) //Enemy is normal mob
				{
				MoveToActor(PlayerCharacter, 40.0f, true);					// Move To player Character
				FVector PlayerLoc = PlayerCharacter->GetActorLocation();
				FVector EnemyLoc = EnemyCharacter->GetActorLocation();
				PlayerLoc.Z = 0;
				EnemyLoc.Z = 0;
				FRotator lookAtRotation = FRotationMatrix::MakeFromX(PlayerLoc - EnemyLoc).Rotator();
				EnemyCharacter->SetActorRotation(lookAtRotation);
				}
				else
				{
					FVector PLoc = PlayerCharacter->GetActorLocation();
					FVector ELoc = EnemyCharacter->GetActorLocation();
					if (FVector::Dist(PLoc, ELoc) > DistanceToPlayerBeforeShooting) //Enemy is too far from player
						{
						MoveToActor(PlayerCharacter, 40.0f, true);					// Move To player Character
						FVector PlayerLoc = PlayerCharacter->GetActorLocation();
						FVector EnemyLoc = EnemyCharacter->GetActorLocation();
						PlayerLoc.Z = 0;
						EnemyLoc.Z = 0;
						FRotator lookAtRotation = FRotationMatrix::MakeFromX(PlayerLoc - EnemyLoc).Rotator();
						EnemyCharacter->SetActorRotation(lookAtRotation);
				}
				else //Player in in range
					{
						Shoot();
					}
			}

		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("PlAYER NOT FOUND"));
		}
			
	}
	else{
		UE_LOG(LogTemp, Warning, TEXT("AI NOT FOUND"));
	}
}

void AEnemyAIController::Shoot()
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	//this->SetActorRotation(FRotator( 0, LaunchDir.Rotation().Yaw, 0));
	if(CanFire)
	{
		GetWorld()->SpawnActor<ADeadBatteryProjectile>(ShootingEnemyProjectile,  this->K2_GetActorLocation(),FRotator( 0, this->K2_GetActorRotation().Yaw, 0), ActorSpawnParams);
		CanFire = false;
	}
}
