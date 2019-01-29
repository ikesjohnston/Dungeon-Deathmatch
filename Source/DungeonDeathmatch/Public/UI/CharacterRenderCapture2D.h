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
class UPhysicsConstraintComponent;
class UStaticMeshComponent;

UCLASS()
class DUNGEONDEATHMATCH_API ACharacterRenderCapture2D : public AActor
{
	GENERATED_BODY()

protected:	

	/** The root mesh the turntable mesh rotates on top of */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* RootMeshComponent;

	/** The turntable mesh the character mesh sits on top of */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* TurntableMeshComponent;

	///** The master skeletal mesh component */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	//UPhysicsConstraintComponent* PhysicsConstraint;

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

	/** The target Z Rotation of the character mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene Capture")
	float RotationTarget;

	/** The alpha value for lerping to the rotation target */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene Capture")
	float RotationTargetLerpAlpha;

	TMap<AActor*, AActor*> DuplicateAttachedActorMap;

public:	
	// Sets default values for this actor's properties
	ACharacterRenderCapture2D();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitializeCharacter(ADungeonCharacter* Character);

	/**
	 * Updates the captured mesh with the specified mesh segment
	 *
	 * @param MeshSegment The mesh segment to alter
	 * @param Mesh The mesh to update the segment to, will use a default mesh if this is nullptr
	 */
	void UpdateMeshSegment(EMeshSegment MeshSegment, USkeletalMesh* NewMesh);

	/**
	 * Attach an actor to the character mesh at the specified socket
	 *
	 * @param Actor The actor to attach
	 * @param SocketName The name of the socket to attach the actor to
	 */
	void AttachActorToSocket(AActor* Actor, FName SocketName, FVector RelativePosition, FRotator RelativeRotation);

	/**
	 * Detach an actor from the character mesh
	 *
	 * @param Actor The actor to detach
	 */
	void DetachActor(AActor* Actor);

	/** Adds yaw input to the character mesh */
	UFUNCTION(BlueprintCallable)
	void AddToTargetRotation(float Rotation);

	/** Stops any ongoing mesh rotation */
	UFUNCTION(BlueprintCallable)
	void StopRotating();
};
