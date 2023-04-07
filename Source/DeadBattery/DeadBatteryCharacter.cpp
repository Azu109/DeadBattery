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
#include "SAdvancedRotationInputBox.h"


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
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	/*Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);*/

	LaunchDirection = FVector(0,0,0);
	IsAiming = false;
	CurrentBloodMeter = MaxBloodMeter;
	CurrentEnergyMeter = MaxEnergyMeter;
	

	
	
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
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			AddControllerPitchInput(15);
			AddControllerYawInput(90);
		}
	}
	FireRateTimer = 1.0f / (FireRate/60.0f);
	CanFire = true;
}

void ADeadBatteryCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	EnergyMeterChange(-DeltaSeconds * EnergyDrainRate); // Slowly Decrease Over Time
	BloodMeterChange(-DeltaSeconds * BloodDrainRate); // Slowly Decrease Over Time
	
	if(!CanFire)
	{
		FireRateTimer -= DeltaSeconds;
		if(FireRateTimer<=0)
		{
			FireRateTimer = 1.0f / (FireRate/60.0f);
			CanFire = true;
			UE_LOG(LogTemp, Warning, TEXT("Fire Rate: %f "),FireRateTimer);
			UE_LOG(LogTemp, Warning, TEXT("DELTA TIME: %f "),DeltaSeconds);
		}
	}

	DeltaTime = DeltaSeconds;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADeadBatteryCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADeadBatteryCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADeadBatteryCharacter::Look);
		
		//Shooting
        EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ADeadBatteryCharacter::Shoot);

		//Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ADeadBatteryCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ADeadBatteryCharacter::StopAiming);
	}

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
		if(!IsAiming)
		{
			Rotation = Controller->GetControlRotation();
			YawRotation= FRotator(0, Rotation.Yaw, 0);
		}
		else
		{
			Rotation = AimRotation;
			YawRotation= FRotator(0, Rotation.Yaw, 0);
		}

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
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
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	//this->SetActorRotation(FRotator( 0, LaunchDir.Rotation().Yaw, 0));
	if(CanFire && CurrentEnergyMeter>=EnergyDrainPerShot && IsAiming)
	{
		CurrentEnergyMeter -= EnergyDrainPerShot;
		GetWorld()->SpawnActor<ADeadBatteryProjectile>(CannonProjectile, GetMesh()->GetSocketLocation("CannonSocket"),FRotator( 0, LaunchDirection.Rotation().Yaw, 0), ActorSpawnParams);
		CanFire = false;
	}
}

void ADeadBatteryCharacter::Aim(const FInputActionValue& Value)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	FHitResult Hit;
	//Controller->CastToPlayerController()->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	
	if (Hit.bBlockingHit){
		if (Hit.GetActor() != NULL){
			LaunchDirection = (Hit.ImpactPoint - GetMesh()->GetSocketLocation("CannonSocket")).GetSafeNormal();
		}
	}

	/*FQuat LaunchRotQT = LaunchDirection.Rotation().Quaternion();
	FQuat ActorRotQT = this->GetActorRotation().Quaternion();

	ActorRotQT = FQuat(FRotator( 0,
		FMath::Lerp(this->GetActorRotation().Yaw , LaunchDirection.Rotation().Yaw, 5.0f*DeltaTime),
		0));
	this->SetActorRotation(ActorRotQT.Rotator());*/

	FRotator From = FRotator( 0,this->GetActorRotation().Yaw,0);
	FRotator To = FRotator( 0,LaunchDirection.Rotation().Yaw,0);
	if(FVector::Dist(Hit.ImpactPoint, this->GetActorLocation()) <150.f)
		AimRotation = FMath::RInterpTo(From, To, DeltaTime,(FVector::Dist(Hit.ImpactPoint, this->GetActorLocation())*5)/1000);
	else
		AimRotation = FMath::RInterpTo(From, To, DeltaTime,10.0);
	this->SetActorRotation(AimRotation);
	
	UE_LOG(LogTemp, Warning, TEXT("DIST: %f"), FVector::Dist(Hit.ImpactPoint, this->GetActorLocation()));
	IsAiming = true;
}

void ADeadBatteryCharacter::StopAiming(const FInputActionValue& Value)
{
	IsAiming = false;
}

void ADeadBatteryCharacter::BloodMeterChange(float Change)
{
	CurrentBloodMeter = CurrentBloodMeter + Change;

	if (CurrentBloodMeter < 0)
		CurrentBloodMeter = 0;

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


