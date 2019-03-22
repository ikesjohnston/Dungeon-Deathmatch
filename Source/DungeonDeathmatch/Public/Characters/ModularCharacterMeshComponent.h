// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "MeshEnums.h"
#include "ModularCharacterMeshComponent.generated.h"

class USkeletalMesh;
class ADungeonCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UModularCharacterMeshComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	/** The skeletal mesh component that stores the helm mesh segment */
	USkeletalMeshComponent* MeshComponentHelm;

	/** The skeletal mesh component that stores the hair mesh segment */
	USkeletalMeshComponent* MeshComponentHair;

	/** The skeletal mesh component that stores the head mesh segment */
	USkeletalMeshComponent* MeshComponentHead;

	/** The skeletal mesh component that stores the left shoulder mesh segment */
	USkeletalMeshComponent* MeshComponentShoulderLeft;

	/** The skeletal mesh component that stores the right shoulder mesh segment */
	USkeletalMeshComponent* MeshComponentShoulderRight;

	/** The skeletal mesh component that stores the torso mesh segment */
	USkeletalMeshComponent* MeshComponentTorso;

	/** The skeletal mesh component that stores the chest armor mesh segment */
	USkeletalMeshComponent* MeshComponentChestArmor;

	/** The skeletal mesh component that stores the left hand mesh segment */
	USkeletalMeshComponent* MeshComponentHandLeft;

	/** The skeletal mesh component that stores the right hand mesh segment */
	USkeletalMeshComponent* MeshComponentHandRight;

	/** The skeletal mesh component that stores the belt mesh segment */
	USkeletalMeshComponent* MeshComponentBelt;

	/** The skeletal mesh component that stores the legs mesh segment */
	USkeletalMeshComponent* MeshComponentLegs;

	/** The skeletal mesh component that stores the leg armor mesh segment */
	USkeletalMeshComponent* MeshComponentLegArmor;

	/** The skeletal mesh component that stores the left mesh segment */
	USkeletalMeshComponent* MeshComponentFootLeft;

	/** The skeletal mesh component that stores the right foot mesh segment */
	USkeletalMeshComponent* MeshComponentFootRight;

	/** Mapping of mesh segment to mesh component, for updating the character mesh with new equipment */
	TMap<EMeshSegment, USkeletalMeshComponent*> MeshComponentMap;

	/** Mapping of mesh segment to default meshes, for updating the character mesh when removing equipment */
	UPROPERTY(EditAnywhere, Category = "Mesh")
	TMap<EMeshSegment, USkeletalMesh*> DefaultMeshMap;

private:
	/** The character that this component is attached to */
	ADungeonCharacter* OwningCharacter;

public:	
	UModularCharacterMeshComponent();

protected:
	virtual void BeginPlay() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:	
	void Initialize();

	/**
	 * Gets the mapping of mesh segment to character's mesh component
	 * @return The mesh segment to component map
	 */
	TMap<EMeshSegment, USkeletalMeshComponent*> GetMeshComponentMap();

	/**
	 * Server call to update a character's mesh segment with a new mesh; used when changing armor equipment.
	 *
	 * @param MeshSegment The mesh segment to alter
	 * @param Mesh The mesh to update the segment to, will use a default mesh if this is nullptr
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUpdateMeshSegment(EMeshSegment MeshSegment, USkeletalMesh* NewMesh);

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
	void MulticastUpdateMeshSegment(EMeshSegment MeshSegment, USkeletalMesh* NewMesh);

};
