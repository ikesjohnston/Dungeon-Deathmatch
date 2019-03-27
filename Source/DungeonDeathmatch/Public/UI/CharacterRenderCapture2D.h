// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "MeshEnums.h"
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

	/** The root mesh that any rotation adjustments will be made on */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* RootMeshComponent;

	USkeletalMeshComponent* MeshComponentMaster;
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

	/** Mapping of mesh segment to mesh component, for updating the character mesh with new equipment */
	TMap<EHumanoidMeshSegment, USkeletalMeshComponent*> MeshComponentMap;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene Capture")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene Capture")
	USceneCaptureComponent2D* SceneCaptureComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene Capture")
	UPointLightComponent* PointLightComponent;

	/** The width of the render texture target to create for scene capture */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene Capture")
	int32 RenderTargetTextureWidth;

	/** The height of the render texture target to create for scene capture */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene Capture")
	int32 RenderTargetTextureHeight;

	/** The render target texture created at runtime for scene capture */
	UTextureRenderTarget2D* RenderTargetTexture;

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

	void InitializeRender(AActor* CapturedActor);

	void UpdateMeshSegment(EHumanoidMeshSegment MeshSegment, USkeletalMesh* NewMesh);

	void AttachActorToSocket(AActor* Actor, FName SocketName, FVector RelativePosition, FRotator RelativeRotation);

	void DetachActor(AActor* Actor);

	UTextureRenderTarget2D* GetRenderTargetTexture() { return RenderTargetTexture; };

	/** Adds yaw input to the character mesh */
	UFUNCTION(BlueprintCallable)
	void AddToTargetRotation(float Rotation);

	/** Stops any ongoing mesh rotation */
	UFUNCTION(BlueprintCallable)
	void StopRotating();
};
