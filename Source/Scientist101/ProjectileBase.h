// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ProjectileBase.generated.h"

// Forward declarations
class UBoxComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;

UCLASS()
class SCIENTIST101_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Box;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileComponent;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ProjectileFX;

	UPROPERTY(EditAnywhere)
	float Speed;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

