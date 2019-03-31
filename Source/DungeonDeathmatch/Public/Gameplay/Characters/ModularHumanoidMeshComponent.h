// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "MeshGlobals.h"
#include "ModularHumanoidMeshComponent.generated.h"

class USkeletalMesh;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UModularHumanoidMeshComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	USkeletalMeshComponent* MeshComponentHelm;
	USkeletalMeshComponent* MeshComponentHair;
	USkeletalMeshComponent* MeshComponentHead;
	USkeletalMeshComponent* MeshComponentShoulderLeft;
	USkeletalMeshComponent* MeshComponentShoulderRight;
	USkeletalMeshComponent* MeshComponentTorso;
	USkeletalMeshComponent* MeshComponentChestArmor;
	USkeletalMeshComponent* MeshComponentHandLeft;
	USkeletalMeshComponent* MeshComponentHandRight;
	USkeletalMeshComponent* MeshComponentBelt;
	USkeletalMeshComponent* MeshComponentLegs;
	USkeletalMeshComponent* MeshComponentLegArmor;
	USkeletalMeshComponent* MeshComponentFootLeft;
	USkeletalMeshComponent* MeshComponentFootRight;

	/** Mapping of mesh segment to mesh component, for updating the mesh with new equipment */
	TMap<EHumanoidMeshSegment, USkeletalMeshComponent*> MeshComponentMap;

	/** Mapping of mesh segment to default meshes, for updating the mesh when removing equipment */
	UPROPERTY(EditAnywhere, Category = "Mesh")
	TMap<EHumanoidMeshSegment, USkeletalMesh*> DefaultMeshMap;

private:
	/** The character that this component is attached to */
	ACharacter* OwningCharacter;

public:	
	UModularHumanoidMeshComponent();

protected:
	virtual void BeginPlay() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:	
	void Initialize();

	TMap<EHumanoidMeshSegment, USkeletalMeshComponent*> GetMeshComponentMap() { return MeshComponentMap; };

	/**
	 * Server call to update a character's mesh segment with a new mesh; used when changing armor equipment.
	 *
	 * @param MeshSegment The mesh segment to alter
	 * @param Mesh The mesh to update the segment to, will use a default mesh if this is nullptr
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUpdateMeshSegment(EHumanoidMeshSegment MeshSegment, USkeletalMesh* NewMesh);

protected:

	/** Sets the character mesh segments to use their assigned default meshes */
	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void InitializeDefaultMeshSegments();

	/**
	 * Updates a character's mesh segment with a new mesh; used when changing armor equipment. Should only be called by the server.
	 *
	 * @param MeshSegment The mesh segment to alter
	 * @param Mesh The mesh to update the segment to, will use a default mesh if this is nullptr
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateMeshSegment(EHumanoidMeshSegment MeshSegment, USkeletalMesh* NewMesh);

};
