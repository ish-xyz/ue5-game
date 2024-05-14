// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

// IMPORTANT!!! Maintain as last include
#include "Protagonist.generated.h"


//Forward declarations
class UInputMappingContext;
class UInputAction;
class USkeletalMeshComponent;
class UAnimMontage;
class UStaticMeshComponent;

UCLASS()
class SCIENTIST101_API AProtagonist : public ACharacter
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	AProtagonist();

	UFUNCTION()
	virtual void HandleOnMontageNotifyBegin(FName notifyName, const FBranchingPointNotifyPayload& branchingPayload);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// INPUT DEFINITIONS
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

	// LOCOMOTION DEFINITIONS
	void MoveAround(const FInputActionValue& Value);

	void LookAround(const FInputActionValue& Value);

	void StartSprint(const FInputActionValue& Value);

	void StopSprint(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Locomotion")
	int GetJumpCount();

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetStamina();

	void PStartJump(const FInputActionValue& Value);

	void PStopJump(const FInputActionValue& Value);

	// COMBAT DEFINITIONS
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	UStaticMeshComponent* MeleeWeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* SwordAttack1Anim;

	void LightSwordAttack();
	void MeeleWeaponCollisionDetector();

	int comboAttackIndex = 0;

private:

	FTimerHandle StaminaTimerHandle;
	FTimerHandle SwordCombatTH;

	float stamina;
	float maxStamina;
	float sprintSpeed;
	float normalSpeed;

	void reduceStamina();
	void increaseStamina();
	void staminaManager(int);

	bool isAttacking();
};
