// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

// IMPORTANT!!! Maintain as last include
#include "MainPlayer.generated.h"


//Forward declarations
class UInputMappingContext;
class UInputAction;
class USkeletalMeshComponent;
class UAnimMontage;
class UStaticMeshComponent;

UCLASS()
class SCIENTIST101_API AMainPlayer : public ACharacter
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

	int comboAttackIndex = 0;
	enum effect {
		none = -1, // fallback reaction
		hit = 0,
		stun = 1,
		knock = 2,
		bleeding = 3,
	};

	struct DamageData {
		float value;
		effect effect;
		bool blockable;
		bool interruptible;
		float effectDuration;
	};

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

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	UStaticMeshComponent* MeleeWeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* MeleeLightAttackAnim;

	// UFUNCTIONS
	UFUNCTION(BlueprintCallable, Category = "Locomotion")
	int GetJumpCount();

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetStamina();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	float GetHealth();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	float GetMaxHealth();

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
	float Heal(float health);
	void TakeDamage(DamageData* dmgInfo);
	void MeeleWeaponCollisionDetector();
	DamageData* newDamageData(float value);

private:

	// Player stats
	float currStamina;
	float maxStamina;
	float sprintSpeed;
	float normalSpeed;
	float meleeDamage;
	float rangeDamage;
	float critDamageMultiplier;
	float critChance;
	float maxHealth;
	float currHealth;
	float healingPerSecond;

	// Timers
	FTimerHandle StaminaTimerHandle;
	FTimerHandle SwordCombatTH;

	// Private Methods
	void reduceStamina();
	void increaseStamina();
	void staminaManager(int);
	bool isAttacking();
	bool isDead();
};
