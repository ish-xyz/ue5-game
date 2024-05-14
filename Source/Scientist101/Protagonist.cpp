// Fill out your copyright notice in the Description page of Project Settings.


#include "Protagonist.h"
#include "Engine.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/ProgressBar.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AProtagonist::AProtagonist()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	sprintSpeed = 1000.0f;
	normalSpeed = 600.0f;
	stamina = 100.0f;
	maxStamina = 100.0f;

	MeleeWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Melee Weapon"));
	MeleeWeaponMesh->SetupAttachment(GetMesh(), FName("socket_hand_r"));

}

// Called when the game starts or when spawned
void AProtagonist::BeginPlay()
{
	Super::BeginPlay();

	// TODO: add widget UI from here

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController) {
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(MainMappingContext, 0);
		}
	}

	UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
	if (pAnimInst != nullptr) {
		pAnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &AProtagonist::HandleOnMontageNotifyBegin);
	}

}

// Called every frame
void AProtagonist::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isAttacking()) {
		MeeleWeaponCollisionDetector();
	}
}

// Called to bind functionality to input
void AProtagonist::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProtagonist::MoveAround);
		EnhancedInputComponent->BindAction(CameraAction, ETriggerEvent::Triggered, this, &AProtagonist::LookAround);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AProtagonist::PStartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AProtagonist::PStopJump);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AProtagonist::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AProtagonist::StopSprint);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AProtagonist::LightSwordAttack);
	}
}

// Locomotion implmentation

void AProtagonist::MoveAround(const FInputActionValue& Value)
{

	if (Controller == nullptr) {
		return;
	}

	if (isAttacking()) {
		return;
	}

	FVector2D currVal = Value.Get<FVector2D>();
	const FRotator rot = Controller->GetControlRotation();

	if (currVal.X != 0.0f) {
		const FVector direction = FRotationMatrix(rot).GetScaledAxis(EAxis::X);
		AddMovementInput(direction, currVal.X);
	}
	if (currVal.Y != 0.0f) {
		const FVector direction = FRotationMatrix(rot).GetScaledAxis(EAxis::Y);
		AddMovementInput(direction, currVal.Y);
	}
}



void AProtagonist::LookAround(const FInputActionValue& Value)
{
	if (Controller == nullptr) {
		return;
	}

	FVector2D currVal = Value.Get<FVector2D>();
	if (currVal.X != 0) {
		AddControllerYawInput(currVal.X);
	}
	if (currVal.Y != 0) {
		AddControllerPitchInput(currVal.Y);
	}

}

void AProtagonist::PStartJump(const FInputActionValue& Value)
{
	if (isAttacking()) {
		return;
	}
	if (GetJumpCount() > 1) {
		LaunchCharacter(FVector(0, 40, 270), false, false);
		return;
	}
	Jump();
}

void AProtagonist::PStopJump(const FInputActionValue& Value)
{
	if (isAttacking()) {
		return;
	}
	StopJumping();
}

void AProtagonist::StartSprint(const FInputActionValue& Value)
{
	// exit conditions
	if (Controller == nullptr) {
		return;
	}

	if (isAttacking()) {
		return;
	}

	float minStamina = 25.0f; // minimum stamina required to sprint
	float staminaExhaustionTimeout = 0.5f; // timeout between every tick of stamina decrease

	if (stamina < minStamina) {
		// TODO: Notify player somehow
		return;
	}


	bool currVal = Value.Get<bool>();
	if (currVal) {
		// TODO: slowly start sprinting
		GetCharacterMovement()->MaxWalkSpeed = sprintSpeed;
	}

	staminaManager(0);
}

void AProtagonist::StopSprint(const FInputActionValue& Value)
{
	if (Controller == nullptr) {
		return;
	}

	// clearing reduce stamina timer if set, and set the increase stamina timer instead
	GetCharacterMovement()->MaxWalkSpeed = normalSpeed;
	staminaManager(1);
}

void AProtagonist::staminaManager(int action) {

	// clear existing timer
	GetWorldTimerManager().ClearTimer(StaminaTimerHandle);
	if (action == -1) {
		// idle stamina
		return;
	}

	if (action == 1) {
		// recharging stamina
		GetWorldTimerManager().SetTimer(
			StaminaTimerHandle, 
			this, 
			&AProtagonist::increaseStamina, 
			1.0f, 
			true, 
			1.0f
		);
	}
	else if (action == 0) {
		// consuming stamina
		GetWorldTimerManager().SetTimer(
			StaminaTimerHandle,
			this,
			&AProtagonist::reduceStamina,
			0.5f,
			true,
			0.0f
		);
	}
}

void AProtagonist::reduceStamina()
{

	if (stamina >= 20.0f) {
		stamina -= 6.0f;

	}
	else {
		// Add side effects here
		StopSprint(NULL);
	}
}

void AProtagonist::increaseStamina()
{
	if (stamina < maxStamina) {
		stamina += 2.0f;
	} else {
		GetWorldTimerManager().ClearTimer(StaminaTimerHandle);
	}
}

int AProtagonist::GetJumpCount()
{
	return JumpCurrentCount;
}

float AProtagonist::GetStamina()
{
	return stamina;
}

// TODO:
// Dodging system with Niagara


// COMBAT SYSTEM
bool AProtagonist::isAttacking()
{
	UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
	if (pAnimInst != nullptr && SwordAttack1Anim != nullptr) {
		return pAnimInst->Montage_IsPlaying(SwordAttack1Anim);
	}
	// return false by default
	return false;
}

void AProtagonist::LightSwordAttack()
{
	if (isAttacking()) {
		comboAttackIndex = 1;
		return;
	}

	UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
	if (pAnimInst != nullptr && SwordAttack1Anim != nullptr) {
		pAnimInst->Montage_Play(SwordAttack1Anim);
	}
}

void AProtagonist::HandleOnMontageNotifyBegin(FName notifyName, const FBranchingPointNotifyPayload& branchingPayload)
{
	comboAttackIndex--;

	if (comboAttackIndex < 0) {
		UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
		if (pAnimInst != nullptr && SwordAttack1Anim != nullptr) {
			pAnimInst->Montage_Stop(0.4f, SwordAttack1Anim);
		}
	}
	return;
}


void AProtagonist::MeeleWeaponCollisionDetector() {

	if (MeleeWeaponMesh == nullptr) {
		return;
	}

	// Setup line trace
	FVector skStart = MeleeWeaponMesh->GetSocketLocation(FName("start"));
	FVector skEnd = MeleeWeaponMesh->GetSocketLocation(FName("end"));
	FHitResult hitResult;
	FCollisionQueryParams traceParams;
	traceParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(hitResult, skStart, skEnd, ECC_Visibility, traceParams);

	DrawDebugLine(GetWorld(), skStart, skEnd, FColor::Red, false, 1, 0, 1);

	// check collision
	if (hitResult.bBlockingHit) {
		AActor* enemy = hitResult.GetActor();
		// try to cast to generalised class of destructubile items
		// call canDestroy()?
		// call ReceiveHit(dmg) // dmg might be ignored based on the type of enemy
		// 
		enemy->Destroy();
	}

}


// Helpers

//if (GEngine) {
//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Reducing Stamina. Current Value: %f"), Stamina));
//}