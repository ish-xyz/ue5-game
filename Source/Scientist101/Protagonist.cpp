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

	// Character Stats

	// Stamina and speed
	sprintSpeed = 1000.0f;
	normalSpeed = 600.0f;
	maxStamina = 100.0f;
	currStamina = 100.0f;

	// Combat
	meleeDamage = 10.0f;
	rangeDamage = 10.0f;
	critDamageMultiplier = 1.3f;
	critChance = 0.5f;
	
	// Health
	maxHealth = 200.0f;
	currHealth = 200.0f;
	healingPerSecond = 10.0f;


	// Melee Weapon
	MeleeWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Melee Weapon"));
	MeleeWeaponMesh->SetupAttachment(GetMesh(), FName("socket_hand_r"));

	// Ranged Weapon?
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
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AProtagonist::MeleeLightAttack);
	}
}

// Locomotion implmentation

void AProtagonist::MoveAround(const FInputActionValue& Value)
{

	if (Controller == nullptr) {
		return;
	}

	if (isAttacking() || isDead()) {
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
	if (isAttacking() || isDead()) {
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
	StopJumping();
}

void AProtagonist::StartSprint(const FInputActionValue& Value)
{
	// exit conditions
	if (Controller == nullptr) {
		return;
	}

	if (isAttacking() || isDead()) {
		return;
	}

	float minStamina = 25.0f; // minimum stamina required to sprint
	float staminaExhaustionTimeout = 0.5f; // timeout between every tick of stamina decrease

	if (currStamina < minStamina) {
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

	if (currStamina >= 20.0f) {
		currStamina -= 6.0f;
	}
	else {
		// Stamina has finished:
		// Add side effects here
		// TODO: implement a subscriber kind of model (where side effects can subscribe to this event).
		StopSprint(NULL);
	}
}

void AProtagonist::increaseStamina()
{
	if (isDead() || currStamina >= maxStamina) {
		staminaManager(-1);
		return;
	}

	currStamina += 2.0f;
}

int AProtagonist::GetJumpCount()
{
	return JumpCurrentCount;
}

float AProtagonist::GetStamina()
{
	return currStamina;
}

// TODO:
// Dodging system with Niagara (quick/instant trail effect)


// COMBAT SYSTEM
AProtagonist::DamageData* AProtagonist::newDamageData(float value)
{

	DamageData* dmg = {};
	dmg->value = value;
	dmg->effectDuration = 0.0f;

	return dmg;
}

bool AProtagonist::isAttacking()
{
	UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
	if (pAnimInst != nullptr && MeleeLightAttackAnim != nullptr) {
		return pAnimInst->Montage_IsPlaying(MeleeLightAttackAnim);
	}
	// return false by default
	return false;
}

bool AProtagonist::isDead()
{
	return currHealth <= 0.0f;
}

void AProtagonist::MeleeLightAttack()
{
	if (isDead()) {
		return;
	}

	if (isAttacking()) {
		comboAttackIndex = 1;
		return;
	}

	UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
	if (pAnimInst != nullptr && MeleeLightAttackAnim != nullptr) {
		pAnimInst->Montage_Play(MeleeLightAttackAnim);
	}
}

void AProtagonist::HandleOnMontageNotifyBegin(FName notifyName, const FBranchingPointNotifyPayload& branchingPayload)
{
	comboAttackIndex--;

	if (comboAttackIndex < 0 || isDead()) {
		UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
		if (pAnimInst != nullptr && MeleeLightAttackAnim != nullptr) {
			pAnimInst->Montage_Stop(0.4f, MeleeLightAttackAnim);
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

		if (AProtagonist* enemyProtagonist = CastChecked<AProtagonist>(enemy)) {
			DamageData* dmg = newDamageData(0.0f);
			enemyProtagonist->TakeDamage(dmg);
		}
		// try to cast to generalised class of destructubile items
		// call canDestroy()?
		// call ReceiveHit(dmg) // dmg might be ignored based on the type of enemy
		// 
		enemy->Destroy();
	}

}

float AProtagonist::GetHealth()
{
	return currHealth;
}

float AProtagonist::GetMaxHealth()
{
	return maxHealth;
}

void AProtagonist::TakeDamage(struct DamageData* dmg)
{
	currHealth -= dmg->value;
	return;
}

float AProtagonist::Heal(float healAmount)
{
	if (currHealth + healAmount > maxHealth) {
		currHealth = maxHealth;
	} else {
		currHealth += healAmount;
	}
	return currHealth;
}


// Helpers

//if (GEngine) {
//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Reducing Stamina. Current Value: %f"), Stamina));
//}