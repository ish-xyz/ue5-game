// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"
#include "Engine/Engine.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick =  false;
	Box = CreateDefaultSubobject<UBoxComponent>(FName("Collision Box"));
	Box->SetBoxExtent(FVector(20.0f, 20.0f, 20.0f));
	Box->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	Box->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	ProjectileFX = CreateDefaultSubobject<UNiagaraSystem>(FName("Projectile FX"));
		ProjectileFX->AttachToComponent(Box, FAttachmentTransformRules::KeepRelativeTransform);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Projectile Mesh"));
	ProjectileMesh->AttachToComponent(Box, FAttachmentTransformRules::KeepRelativeTransform);
	ProjectileMesh->SetCollisionProfileName(TEXT("NoCollision"));

	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Projectile Component"));
	ProjectileComponent->InitialSpeed = Speed;
	ProjectileComponent->MaxSpeed = Speed;
	ProjectileComponent->ProjectileGravityScale = 0.0f;
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	Box->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
}

void AProjectileBase::onHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Hit")));
	}

	//TODO: add damage dealer

	this->Destroy();
}
// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

