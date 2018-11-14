// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystemComponent.h>
#include "StatusComponent.h"
#include <PhysicsEngine/RadialForceComponent.h>


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	SetRootComponent(ProjectileMesh);
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnProjectileHit);

	TrailParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailParticles"));
	TrailParticles->SetupAttachment(ProjectileMesh);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	SetReplicates(true);
	SetReplicateMovement(true);

	Damage = 100;
	Force = 50000;
	ForceRadius = 250;

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectile::Multicast_OnProjectileHit_Implementation()
{
	PostProjectileHit();
}

void AProjectile::PostProjectileHit_Implementation()
{
	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticles, GetActorLocation());
	}
}

void AProjectile::OnProjectileHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Role < ROLE_Authority)
		return;
	Multicast_OnProjectileHit();
}

