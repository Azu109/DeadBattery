// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
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
	EnemyType = 0;
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
			if (EnemyType == 0) //Enemy is normal mob
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
				if (FVector::Dist(PLoc, ELoc) > 50) //Enemy is too far from player
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
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

					//GetWorld()->SpawnActor<ADeadBatteryProjectile>(CannonProjectile, GetSphereMesh(), FRotator( 0, LaunchDirection.Rotation().Yaw, 0), ActorSpawnParams);
					CanFire = false;
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

void AEnemyAIController::SetEnemyType(int Type)
{
	EnemyType = Type;
}

int AEnemyAIController::GetEnemyType()
{
	return EnemyType;
}