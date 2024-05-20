// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Damageable.h"

// IMPORTANT!!! Maintain as last include
#include "MainPlayer.generated.h"


//Forward declarations
class UInputMappingContext;
class UInputAction;
class USkeletalMeshComponent;
class UAnimMontage;
class UStaticMeshComponent;
class AProjectileBase;

UCLASS()
class SCIENTIST101_API AMainPlayer : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	AMainPlayer();

	UFUNCTION()
	virtual void HandleOnMontageNotifyBegin(FName notifyName, const FBranchingPointNotifyPayload& branchingPayload);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	float currStamina;
	float maxStamina;
	float sprintSpeed;
	float runSpeed;
	float attackSpeed;
	float meleeDamage;
	float rangeDamage;
	float critDamageMultiplier;
	float critChance;
	float maxHealth;
	float currHealth;
	float healsPerSecond;
	bool  invincible;
	int   attackComboTracker;

	// UPROPS
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* MainMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* CameraAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* RangedAttackAction;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AProjectileBase> ProjectileActor;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	UStaticMeshComponent* MeleeWeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* MeleeLightAttackAnim;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	UAnimMontage* OnGoingAnimation;

	// UFUNCTIONS
	UFUNCTION(BlueprintCallable, Category = "Locomotion")
	int GetJumpCount();

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetStamina();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual float GetHealth() override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual float GetMaxHealth() override;

	// Other Methods

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Bound to enhanced Inputs
	void MoveAround(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	void StartSprint(const FInputActionValue& Value);
	void StopSprint(const FInputActionValue& Value);
	void PStartJump(const FInputActionValue& Value);
	void PStopJump(const FInputActionValue& Value);
	void MeleeLightAttack();

	// Others
	virtual float Heal(float health) override;
	virtual bool  ReceiveDamage(Damage dmg) override;
	void MeeleWeaponCollisionDetector();

private:

	// Timers
	IDamageable::Damage damageInfo = {};
	FTimerHandle StaminaTimerHandle;
	FTimerHandle SwordCombatTH;

	// Private Methods
	void reduceStamina();
	void increaseStamina();
	void staminaManager(int);
	bool isAttacking();
	bool isDead();
	void interruptAction();
	bool isInvincible();
	bool isBlocking();
};
