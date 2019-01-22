// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonCharacter.h"
#include "CharacterRenderCapture2D.generated.h"

class USkeletalMesh;
class USkeletalMeshComponent;
class UCameraComponent;
class USceneCaptureComponent2D;

UCLASS()
class DUNGEONDEATHMATCH_API ACharacterRenderCapture2D : public AActor
{
	GENERATED_BODY()

protected:	
	/********************************************************* BEGIN CHARACTER MESH SEGMENT VARIABLES *********************************************************/

	/** The master skeletal mesh component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentMaster;

	/** The skeletal mesh component that stores the helm mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentHelm;

	/** The skeletal mesh component that stores the hair mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentHair;

	/** The skeletal mesh component that stores the head mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentHead;

	/** The skeletal mesh component that stores the left shoulder mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentShoulderLeft;

	/** The skeletal mesh component that stores the right shoulder mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentShoulderRight;

	/** The skeletal mesh component that stores the torso mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentTorso;

	/** The skeletal mesh component that stores the chest armor mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentChestArmor;

	/** The skeletal mesh component that stores the left hand mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentHandLeft;

	/** The skeletal mesh component that stores the right hand mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentHandRight;

	/** The skeletal mesh component that stores the belt mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentBelt;

	/** The skeletal mesh component that stores the legs mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentLegs;

	/** The skeletal mesh component that stores the leg armor mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentLegArmor;

	/** The skeletal mesh component that stores the left mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentFootLeft;

	/** The skeletal mesh component that stores the right foot mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentFootRight;

	/** Mapping of mesh segment to mesh component, for updating the character mesh with new equipment */
	TMap<EMeshSegment, USkeletalMeshComponent*> MeshComponentMap;

	/********************************************************* END CHARACTER MESH SEGMENTS VARIABLES **********************************************************/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene Capture")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene Capture")
	USceneCaptureComponent2D* SceneCaptureComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene Capture")
	UPointLightComponent* PointLightComponent;

	/**
	 * The degrees per second the model will turn when given maximum horizontal mouse input
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene Capture")
	float InputRotationRate;

	/** The character that's appearance is being dupilcated for this scene */
	ADungeonCharacter* DisplayedCharacter;

public:	
	// Sets default values for this actor's properties
	ACharacterRenderCapture2D();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Event for when a new piece of armor is equipped to the displayed character 
	 *
	 * @param Armor The piece of armor that was equipped
	 */
	UFUNCTION()
	void OnArmorEquipped(AArmor* Armor);

	/** 
	 * Updates the duplicated mesh with the mapped skeletal meshes
	 *
	 * @param MeshMap The mapping of mesh segment to skeletal mesh
	 */
	void UpdateMeshSegments(TMap<EMeshSegment, USkeletalMesh*> MeshMap);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitializeCharacter(ADungeonCharacter* Character);

	/**
	 * Rotates the captured character given some mouse axis input
	 */
	UFUNCTION(BlueprintCallable)
	void RotateCharacter(float RotationInput, float DeltaTime);
};
