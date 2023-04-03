// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "DeadBattery/DeadBatteryCharacter.h"
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

	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn());						// Get Enemy Character it is Controlling

	if (EnemyCharacter != NULL) {
		ADeadBatteryCharacter* PlayerCharacter = Cast<ADeadBatteryCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));		// Get Player Character

		if (PlayerCharacter != NULL) {
			MoveToActor(PlayerCharacter, 40.0f, true);					// Move To player Character
			FVector PlayerLoc = PlayerCharacter->GetActorLocation();
			FVector EnemyLoc = EnemyCharacter->GetActorLocation();
			PlayerLoc.Z = 0;
			EnemyLoc.Z = 0;
			FRotator lookAtRotation = FRotationMatrix::MakeFromX(PlayerLoc - EnemyLoc).Rotator();
			EnemyCharacter->SetActorRotation(lookAtRotation);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("PlAYER NOT FOUND"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("AI NOT FOUND"));
	}
}
