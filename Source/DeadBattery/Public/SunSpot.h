/// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeadBattery/DeadBatteryCharacter.h"
#include "GameFramework/Actor.h"
#include "SunSpot.generated.h"

UCLASS()
class DEADBATTERY_API ASunSpot : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASunSpot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* CapsuleComp;

	UPROPERTY(EditAnywhere, Category = "Effect")
		float BloodLoss = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Effect")
		float EnergyGain = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Effect")
		float TimeInterval = 5.0f;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void PlayerUnderSun(ADeadBatteryCharacter* PlayerCharacter);

	FTimerHandle UnderSunTimerHandle;
	FTimerDelegate TimerDel;


};
