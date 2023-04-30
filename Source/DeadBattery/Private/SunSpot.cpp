// Fill out your copyright notice in the Description page of Project Settings.


#include "SunSpot.h"

#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SpotLightComponent.h"
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

	SpotLightComp = CreateDefaultSubobject<USpotLightComponent>("SunSpot");

	SpotLightComp->Intensity = 1000000.f;
	SpotLightComp->InnerConeAngle = 13.f;
	SpotLightComp->OuterConeAngle = 13.3f;
	SpotLightComp->AttenuationRadius = 5000.f;
	SpotLightComp->SetRelativeRotation(FRotator(0,-90,0));
	SpotLightComp->Mobility = EComponentMobility::Movable;

	
	RootComponent = CapsuleComp;

	CapsuleComp->Mobility = EComponentMobility::Movable;

	isEnabled = true;
}

// Called when the game starts or when spawned
void ASunSpot::BeginPlay()
{
	Super::BeginPlay();

	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &ASunSpot::OnOverlapBegin);
	CapsuleComp->OnComponentEndOverlap.AddDynamic(this, &ASunSpot::OnOverlapEnd);

	SpotLightComp->SetupAttachment(CapsuleComp);
	SpotLightComp->bAffectsWorld = true;
	SpotLightComp->Activate(true);
	//SpotLightComp->SetWorldLocation(FVector(CapsuleComp->GetRelativeLocation()));
}

// Called every frame
void ASunSpot::Tick(float DeltaTime)
{
	
	Super::Tick(DeltaTime);
	SpotLightComp->SetVisibility(false);
	if(SunSpotTimer>=0)
		SunSpotTimer -=DeltaTime;

	SpotLightComp->InnerConeAngle = 13.0*(SunSpotTimer/10);
	SpotLightComp->OuterConeAngle = 13.3*(SunSpotTimer/10);
	CapsuleComp->SetCapsuleRadius(200 * (SunSpotTimer/10));
	
	SpotLightComp->SetVisibility(true);
}



void ASunSpot::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (ADeadBatteryCharacter* PlayerCharacter = Cast<ADeadBatteryCharacter>(OtherActor))
	{

		
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Overlap Begin");
		//SpotLightComp->SetWorldLocation(PlayerCharacter->GetActorLocation());
		//CapsuleComp->SetCapsuleRadius(200.0f*(SunSpotTimer/10));
		TimerDel.BindUFunction(this, FName("PlayerUnderSun"), PlayerCharacter);
		GetWorldTimerManager().SetTimer(UnderSunTimerHandle, TimerDel, TimeInterval, true, TimeInterval);
	}

}

void ASunSpot::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Overlap End");
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
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Overlap Begin");
	if(!PlayerCharacter->IsUnderSun)
		PowerUpAudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, PowerUpSFX,this->K2_GetActorLocation(),this->GetActorRotation());
	
	PlayerCharacter->IsUnderSun = true;
	PlayerCharacter->BloodMeterChange(-BloodLoss);
	PlayerCharacter->EnergyMeterChange(EnergyGain);
}
