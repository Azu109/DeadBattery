// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeadBatteryCharacter.h"

#include "DeadBatteryProjectile.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MainSaveGame.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"


//////////////////////////////////////////////////////////////////////////
// ADeadBatteryCharacter

ADeadBatteryCharacter::ADeadBatteryCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 2.0f;
	CameraBoom->TargetArmLength = 500.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	/*Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);*/

	CollisionCompCap = GetCapsuleComponent();
	CollisionCompCap->SetCapsuleRadius(50.0f);

	LaunchDirection = FVector(0, 0, 0);
	IsAiming = false;
	CurrentBloodMeter = MaxBloodMeter;
	CurrentEnergyMeter = MaxEnergyMeter;

	Timer = 0.0f;
	Score = 0.0f;
	MeleeCooldownTimer = 0;

	ShieldSpawned = false;
	Shield = NULL;
	IsTakingDamage = false;
	TakingDamageTimer = .1;
}

void ADeadBatteryCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;
	GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = true;


	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			AddControllerPitchInput(15);
			AddControllerYawInput(90);
		}
	}
	FireRateTimer = 1.0f / (FireRate / 60.0f);
	CanFire = true;

	CollisionCompCap->OnComponentHit.AddDynamic(this, &ADeadBatteryCharacter::OnHit);
	IsUnderSun = false;

	LoadGame();
}

void ADeadBatteryCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Timer += DeltaSeconds;
	EnergyMeterChange(-DeltaSeconds * EnergyDrainRate); // Slowly Decrease Over Time
	BloodMeterChange(-DeltaSeconds * BloodDrainRate); // Slowly Decrease Over Time

	if (!CanFire)
	{
		FireRateTimer -= DeltaSeconds;
		if (FireRateTimer <= 0)
		{
			FireRateTimer = 1.0f / (FireRate / 60.0f);
			CanFire = true;
			UE_LOG(LogTemp, Warning, TEXT("Fire Rate: %f "), FireRateTimer);
			UE_LOG(LogTemp, Warning, TEXT("DELTA TIME: %f "), DeltaSeconds);
		}
	}

	if(IsTakingDamage)
	{
		TakingDamageTimer-= DeltaSeconds;
		if(TakingDamageTimer<=0)
		{
			TakingDamageTimer = .1;
			IsTakingDamage = false;
		}
	}

	DeltaTime = DeltaSeconds;

	if (MeleeCooldownTimer > 0)
	{
		MeleeCooldownTimer -= DeltaSeconds;
		if(MeleeCooldownTimer <=1)
			IsMeleeAttacking = false;
	}
	else
	{
		IsMeleeHitting = false;
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADeadBatteryCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADeadBatteryCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this,
		                                   &ADeadBatteryCharacter::StopMoving);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADeadBatteryCharacter::Look);

		//Shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ADeadBatteryCharacter::Shoot);

		//Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ADeadBatteryCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this,
		                                   &ADeadBatteryCharacter::StopAiming);

		//Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this,
		                                   &ADeadBatteryCharacter::StartSprinting);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this,
		                                   &ADeadBatteryCharacter::StopSprinting);

		//Shield
		EnhancedInputComponent->BindAction(ShieldAction, ETriggerEvent::Triggered, this,
		                                   &ADeadBatteryCharacter::StartShield);
		EnhancedInputComponent->BindAction(ShieldAction, ETriggerEvent::Completed, this,
		                                   &ADeadBatteryCharacter::StopShield);
	}
}

void ADeadBatteryCharacter::StopMoving(const FInputActionValue& Value)
{
	StrafingValue = 0;
	WalkingBackValue = 0;
}

void ADeadBatteryCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		FRotator Rotation;
		FRotator YawRotation;
		// find out which way is forward
		//if(!IsAiming)
		//{
		Rotation = Controller->GetControlRotation();
		YawRotation = FRotator(0, Rotation.Yaw, 0);
		//}
		//else
		//{
		//	Rotation = AimRotation;
		//	YawRotation= FRotator(0, Rotation.Yaw, 0);
		//}

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		if (MeleeCooldownTimer <= 0)
		{
			
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);

			if (IsAiming)
			{
				if (AimRotation.Yaw > 90 && AimRotation.Yaw < 180)
				{
					StrafingValue = MovementVector.Y;
					WalkingBackValue = MovementVector.X;
				}
				else if (AimRotation.Yaw < -0 && AimRotation.Yaw > -90)
				{
					StrafingValue = -MovementVector.Y;
					WalkingBackValue = -MovementVector.X;
				}
				else if (AimRotation.Yaw < -90 && AimRotation.Yaw > -180)
				{
					//Front
					StrafingValue = MovementVector.X;
					WalkingBackValue = -MovementVector.Y;
				}
				else if (AimRotation.Yaw > 0 && AimRotation.Yaw < 90)
				{
					//Back
					StrafingValue = -MovementVector.X;
					WalkingBackValue = MovementVector.Y;
				}
			}
		}
	}
}

void ADeadBatteryCharacter::StartSprinting(const FInputActionValue& Value)
{
	IsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = 700.f;
}

void ADeadBatteryCharacter::StopSprinting(const FInputActionValue& Value)
{
	IsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
}

void ADeadBatteryCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		// AddControllerYawInput(LookAxisVector.X);
		// AddControllerPitchInput(LookAxisVector.Y); 
	}
}

void ADeadBatteryCharacter::Shoot(const FInputActionValue& Value)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;


	//this->SetActorRotation(FRotator( 0, LaunchDir.Rotation().Yaw, 0));
	if (CanFire && CurrentEnergyMeter >= EnergyDrainPerShot && IsAiming && !ShieldSpawned)
	{
		CurrentEnergyMeter -= EnergyDrainPerShot;
		GetWorld()->SpawnActor<ADeadBatteryProjectile>(CannonProjectile, GetMesh()->GetSocketLocation("CannonSocket"),
		                                               FRotator(0, GetActorRotation().Yaw + 6, 0),
		                                               ActorSpawnParams);

		//UE_LOG(LogTemp, Warning, TEXT("Rotation Needed: %f %f %f"),  this->GetActorRotation().Pitch,this->GetActorRotation().Yaw,this->GetActorRotation().Roll);
		//UE_LOG(LogTemp, Warning, TEXT("Rotation Got: %f %f %f"), GetMesh()->GetSocketRotation("CannonSocket").Pitch,GetMesh()->GetSocketRotation("CannonSocket").Yaw,GetMesh()->GetSocketRotation("CannonSocket").Roll);

		UGameplayStatics::SpawnSoundAtLocation(this, CannonFireSFX, this->K2_GetActorLocation(),
		                                       this->GetActorRotation(), FMath::RandRange(0.4, 0.6),
		                                       FMath::RandRange(0.9, 1.1));

		CanFire = false;
	}
	else if (CanFire && CurrentEnergyMeter < EnergyDrainPerShot)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, NoBulletSFX, this->K2_GetActorLocation(),
		                                       this->GetActorRotation());
		CanFire = false;
	}

	if (!IsAiming && MeleeCooldownTimer <= 0 && !ShieldSpawned && CurrentEnergyMeter >= 2 * EnergyDrainPerShot)
	{
		CurrentEnergyMeter -= 2 * EnergyDrainPerShot;
		IsMeleeAttacking = true;
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		FHitResult Hit;
		PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		UGameplayStatics::SpawnSoundAtLocation(this, MeleeSFX, this->K2_GetActorLocation(),
		                                       this->GetActorRotation());
		if (Hit.bBlockingHit)
		{
			if (Hit.GetActor() != NULL)
			{
				LaunchDirection = (Hit.ImpactPoint - GetMesh()->GetSocketLocation("CannonSocket")).GetSafeNormal();
			}
		}

		//FRotator From = FRotator( 0,this->GetActorRotation().Yaw,0);
		FRotator To = FRotator(0, LaunchDirection.Rotation().Yaw, 0);
		/*if(FVector::Dist(Hit.ImpactPoint, this->GetActorLocation()) <150.f)
			AimRotation = FMath::RInterpTo(From, To, DeltaTime,(FVector::Dist(Hit.ImpactPoint, this->GetActorLocation())*5)/1000);
		else
			AimRotation = FMath::RInterpTo(From, To, DeltaTime,10.0);*/

		//this->SetActorRotation(To);

		// Basic Line Trace Detection for Melee (TEST)
		FHitResult HitObject;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		GetWorld()->LineTraceSingleByChannel(HitObject, GetMesh()->GetSocketLocation("MeleeStartSocket"),
		                                     GetMesh()->GetSocketLocation("MeleeEndSocket"),
		                                     ECollisionChannel::ECC_Camera, Params, FCollisionResponseParams());

		if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(HitObject.GetActor()))
		{
			//UGameplayStatics::SpawnSoundAtLocation(this, EnemyHitSFX,this->K2_GetActorLocation(),this->GetActorRotation(),FMath::RandRange(0.8,1.2),FMath::RandRange(0.5,1.5));

			IsMeleeHitting = true;
			float EnemyHealth = Enemy->CurrentHealth;
			if (EnemyHealth != 0)
			{
				EnemyHealth = -10.0f;
				Enemy->IsFlinching = true;
				Enemy->FlinchTimer = Enemy->FlinchAnimDuration;

				ADeadBatteryCharacter* Player = Cast<ADeadBatteryCharacter>(
					UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
				Player->BloodMeterChange(Enemy->MaxHealth / 2.5f);
				Player->Score += Player->Timer / 60.f + 1.f;
				Player->SaveGame();

				Enemy->CurrentHealth = EnemyHealth;
				//UE_LOG(LogTemp, Warning, TEXT("ENEMY HIT"));
				
			}
		}
		/*DrawDebugLine(GetWorld(), GetMesh()->GetSocketLocation("MeleeStartSocket"),
		              GetMesh()->GetSocketLocation("MeleeEndSocket"), FColor::Red, false, 5.0f);*/


		MeleeCooldownTimer = MeleeCooldownDuration;
	}
}

void ADeadBatteryCharacter::Aim(const FInputActionValue& Value)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	FHitResult Hit;
	//Controller->CastToPlayerController()->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		if (Hit.GetActor() != NULL)
		{
			LaunchDirection = (Hit.ImpactPoint - GetMesh()->GetSocketLocation("CannonSocket")).GetSafeNormal();
		}
	}

	/*FQuat LaunchRotQT = LaunchDirection.Rotation().Quaternion();
	FQuat ActorRotQT = this->GetActorRotation().Quaternion();

	ActorRotQT = FQuat(FRotator( 0,
		FMath::Lerp(this->GetActorRotation().Yaw , LaunchDirection.Rotation().Yaw, 5.0f*DeltaTime),
		0));
	this->SetActorRotation(ActorRotQT.Rotator());*/

	FRotator From = FRotator(0, this->GetActorRotation().Yaw, 0);
	FRotator To = FRotator(0, LaunchDirection.Rotation().Yaw, 0);
	if (FVector::Dist(Hit.ImpactPoint, this->GetActorLocation()) < 150.f)
		AimRotation = FMath::RInterpTo(From, To, DeltaTime,
		                               (FVector::Dist(Hit.ImpactPoint, this->GetActorLocation()) * 5) / 1000);
	else
		AimRotation = FMath::RInterpTo(From, To, DeltaTime, 10.0);
	this->SetActorRotation(AimRotation);

	//UE_LOG(LogTemp, Warning, TEXT("DIST: %f"), FVector::Dist(Hit.ImpactPoint, this->GetActorLocation()));

	if (!IsAiming)
		AimGuide = GetWorld()->SpawnActor<AActor>(AimGuideClass, Hit.ImpactPoint, FRotator(0));
	if (AimGuide != nullptr)
		AimGuide->SetActorLocation(Hit.ImpactPoint);
	IsAiming = true;

	UE_LOG(LogTemp, Warning, TEXT("AIM Rotation : %f "), AimRotation.Yaw);
}

void ADeadBatteryCharacter::StopAiming(const FInputActionValue& Value)
{
	IsAiming = false;
	AimGuide->Destroy();
	StrafingValue = 0;
	WalkingBackValue = 0;
}


// Enables Shield on Keypress and reduces Energy
void ADeadBatteryCharacter::StartShield(const FInputActionValue& Value)
{
	if (IsUnderSun)
	{
		StopShield(Value);
		return;
	}

	if (!ShieldSpawned)
	{
		ShieldAudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, ShieldSFX, this->K2_GetActorLocation(),
		                                                              this->GetActorRotation());
		FVector SpineSocketLoc = GetMesh()->GetSocketLocation("ShieldSocket");
		Shield = GetWorld()->SpawnActor<AActor>(PlayerShield, SpineSocketLoc, FRotator(0));
		FAttachmentTransformRules a = FAttachmentTransformRules::SnapToTargetIncludingScale;
		a.RotationRule = EAttachmentRule::KeepWorld;
		Shield->AttachToComponent(GetMesh(), a, "ShieldSocket");
		ShieldSpawned = true;
	}
	if (CurrentEnergyMeter <= 0)
	{
		StopShield(Value);
	}
	EnergyMeterChange(-ShieldDrainRate);
}

// Destroy Shield when player releases button
void ADeadBatteryCharacter::StopShield(const FInputActionValue& Value)
{
	if (Shield != nullptr)
		Shield->Destroy();

	ShieldSpawned = false;

	if (ShieldAudioComponent != nullptr)
		if (ShieldAudioComponent->IsPlaying())
		{
			ShieldAudioComponent->Stop();
		}
}

void ADeadBatteryCharacter::BloodMeterChange(float Change)
{
	if(Change < -2)
		IsTakingDamage = true;
	else
	{
		IsTakingDamage = false;
	}
	CurrentBloodMeter = CurrentBloodMeter + Change;

	if (CurrentBloodMeter <= 0)
	{
		CurrentBloodMeter = 0;
		SaveGame();
		UGameplayStatics::OpenLevel(GetWorld(), FName("GameOverLevel")); //Should be game over
	}

	else if (CurrentBloodMeter > MaxBloodMeter)
		CurrentBloodMeter = MaxBloodMeter;
	//UE_LOG(LogTemp, Warning, TEXT("Blood: %f"), CurrentBloodMeter);
}

void ADeadBatteryCharacter::EnergyMeterChange(float Change)
{
	CurrentEnergyMeter = CurrentEnergyMeter + Change;

	if (CurrentEnergyMeter < 0)
		CurrentEnergyMeter = 0;

	else if (CurrentEnergyMeter > MaxEnergyMeter)
		CurrentEnergyMeter = MaxEnergyMeter;

	//UE_LOG(LogTemp, Warning, TEXT("Energy: %f"), CurrentEnergyMeter);
}


void ADeadBatteryCharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                  FVector NormalImpulse, const FHitResult& Hit)
{
	/*UE_LOG(LogTemp, Warning, TEXT("PLAYER HITTING!!!"));*/

	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor);
	// Only add impulse and destroy projectile if we hit a physics
	if (Enemy != nullptr && MeleeCooldownTimer > 0)
	{
		float EnemyHealth = Enemy->CurrentHealth;
		if (EnemyHealth <= 0)
			return;
		EnemyHealth = -10.0f;
		Enemy->IsFlinching = true;
		Enemy->FlinchTimer = Enemy->FlinchAnimDuration;
		if (EnemyHealth <= 0)
		{
			ADeadBatteryCharacter* Player = Cast<ADeadBatteryCharacter>(
				UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			Player->BloodMeterChange(Enemy->MaxHealth / 2.5f);
			Player->Score += Player->Timer / 60.f + 1.f;
			Player->SaveGame();
			IsMeleeHitting = true;
		}
		Enemy->CurrentHealth = EnemyHealth;
	}
}


void ADeadBatteryCharacter::LoadGame()
{
	UMainSaveGame* SaveGameInstance = Cast<UMainSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UMainSaveGame::StaticClass()));

	if (UGameplayStatics::DoesSaveGameExist(TEXT("DeadBatterySave"), 0) == false)
	{
		UGameplayStatics::CreateSaveGameObject(TSubclassOf<UMainSaveGame>());
	}
	else
	{
		SaveGameInstance = Cast<UMainSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("DeadBatterySave"), 0));
	}

	HighScore = SaveGameInstance->HighScore;
}

void ADeadBatteryCharacter::SaveGame()
{
	UMainSaveGame* SaveGameInstance = Cast<UMainSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UMainSaveGame::StaticClass()));

	if (Score > HighScore)
		HighScore = Score;

	SaveGameInstance->HighScore = HighScore;
	SaveGameInstance->LastScore = Score;

	if (UGameplayStatics::DoesSaveGameExist(TEXT("DeadBatterySave"), 0) == false)
	{
		UGameplayStatics::CreateSaveGameObject(TSubclassOf<UMainSaveGame>());
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("DeadBatterySave"), 0);
	}
	else
	{
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("DeadBatterySave"), 0);
	}
}
