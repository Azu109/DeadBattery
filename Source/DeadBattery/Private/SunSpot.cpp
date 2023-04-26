// Fill out your copyright notice in the Description page of Project Settings.


#include "SunSpot.h"

#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "DeadBattery/DeadBatteryCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASunSpot::ASunSpot()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	
	CapsuleComp->SetCapsuleRadius(200.0f);
	CapsuleComp->SetCapsuleHalfHeight(2000.0f);

	/*
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Comp"));
	RootComponent->SetupAttachment(CapsuleComp);
	*/

	RootComponent = CapsuleComp;

	

}

// Called when the game starts or when spawned
void ASunSpot::BeginPlay()
{
	Super::BeginPlay();

	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &ASunSpot::OnOverlapBegin);
	CapsuleComp->OnComponentEndOverlap.AddDynamic(this, &ASunSpot::OnOverlapEnd);

}

// Called every frame
void ASunSpot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void ASunSpot::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Overlap Begin");

	if (ADeadBatteryCharacter* PlayerCharacter = Cast<ADeadBatteryCharacter>(OtherActor))
	{
		TimerDel.BindUFunction(this, FName("PlayerUnderSun"), PlayerCharacter);
		GetWorldTimerManager().SetTimer(UnderSunTimerHandle, TimerDel, TimeInterval, true, TimeInterval);
	}

}

void ASunSpot::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Overlap End");
	if (ADeadBatteryCharacter* PlayerCharacter = Cast<ADeadBatteryCharacter>(OtherActor))
	{
		if(PlayerCharacter->IsUnderSun != false)
			UGameplayStatics::SpawnSoundAtLocation(this, PowerDownSFX,this->K2_GetActorLocation(),this->GetActorRotation());

		if(PowerUpAudioComponent !=nullptr)
			PowerUpAudioComponent->Stop();
		
		PlayerCharacter->IsUnderSun = false;
		GetWorldTimerManager().ClearTimer(UnderSunTimerHandle);
	}
}

void ASunSpot::PlayerUnderSun(ADeadBatteryCharacter* PlayerCharacter)
{
	if(!PlayerCharacter->IsUnderSun)
		PowerUpAudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, PowerUpSFX,this->K2_GetActorLocation(),this->GetActorRotation());
	
	PlayerCharacter->IsUnderSun = true;
	PlayerCharacter->BloodMeterChange(-BloodLoss);
	PlayerCharacter->EnergyMeterChange(EnergyGain);
}
