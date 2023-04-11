// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "DeadBatteryProjectile.h"
#include "VectorTypes.h"
#include "kismet/GameplayStatics.h"

AEnemyAIController::AEnemyAIController()
{
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn()); // Get Enemy Character it is Controlling

	if (EnemyCharacter == NULL)
		return;


	if (EnemyCharacter != NULL)
	{
		ADeadBatteryCharacter* PlayerCharacter = Cast<ADeadBatteryCharacter>(
			UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)); // Get Player Character

		if (PlayerCharacter != NULL)
		{
			if (EnemyCharacter->EnemyType == EEnemyType::ET_Melee) //Enemy is normal mob
			{
				MoveToActor(PlayerCharacter, 40.0f, true); // Move To player Character
				FVector PlayerLoc = PlayerCharacter->GetActorLocation();
				FVector EnemyLoc = EnemyCharacter->GetActorLocation();
				PlayerLoc.Z = 0;
				EnemyLoc.Z = 0;
				FRotator lookAtRotation = FRotationMatrix::MakeFromX(PlayerLoc - EnemyLoc).Rotator();
				EnemyCharacter->SetActorRotation(lookAtRotation);
			}
			else if (EnemyCharacter->EnemyType == EEnemyType::ET_Shooting)
			{
				FVector PlayerLoc = PlayerCharacter->GetActorLocation();
				FVector EnemyLoc = EnemyCharacter->GetActorLocation();
				PlayerLoc.Z = 0;
				EnemyLoc.Z = 0;
				FRotator lookAtRotation = FRotationMatrix::MakeFromX(PlayerLoc - EnemyLoc).Rotator();
				EnemyCharacter->SetActorRotation(lookAtRotation);
				if (FVector::Dist(PlayerLoc, EnemyLoc) > EnemyCharacter->DistanceToPlayerBeforeShooting)
				{
					MoveToActor(PlayerCharacter, 40.0f, true);
					EnemyCharacter->InRange = false;
					EnemyCharacter->FireRateTimer = EnemyCharacter->FirstTimeShotTimer;
				}
				else
				{
					StopMovement();
					Shoot(EnemyCharacter,PlayerCharacter);
					EnemyCharacter->InRange = true;
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PlAYER NOT FOUND"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AI NOT FOUND"));
	}
}

void AEnemyAIController::Shoot(AEnemyCharacter* EnemyCharacter, ADeadBatteryCharacter* Player)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	//this->SetActorRotation(FRotator( 0, LaunchDir.Rotation().Yaw, 0));
	if (EnemyCharacter->CanFire)
	{
		GetWorld()->SpawnActor<ADeadBatteryProjectile>(EnemyCharacter->ShootingEnemyProjectile,
		                                               EnemyCharacter->GetMesh()->GetSocketLocation("CannonSocket"),
		                                               FRotator(0, (Player->GetActorLocation() - this->K2_GetActorLocation()).Rotation().Yaw, 0),
		                                               ActorSpawnParams);
		EnemyCharacter->CanFire = false;
	}
}
