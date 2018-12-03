// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UStaticMeshComponent;
class UParticleSystemComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class URadialForceComponent;

UCLASS()
class DUNGEONDEATHMATCH_API AProjectile : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	UParticleSystemComponent* TrailParticles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	float Force;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	float ForceRadius;

public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnProjectileHit();

	UFUNCTION(BlueprintNativeEvent)
	void PostProjectileHit();
};
