// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MeshEnums.h"
#include "RenderCaptureComponent.generated.h"

class ACharacterRenderCapture2D;
class ADungeonCharacter;
class USkeletalMesh;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API URenderCaptureComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	/** The 2DRenderCapture class for displaying the character mesh in the UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Render Capture")
	TSubclassOf<ACharacterRenderCapture2D> RenderCaptureClass;

	/** The 2DRenderCapture actor for displaying the character mesh in the UI */
	UPROPERTY()
	ACharacterRenderCapture2D* RenderCaptureActor;

private:
	/** The character that this component is attached to */
	ADungeonCharacter* OwningCharacter;

public:	
	URenderCaptureComponent();

protected:
	virtual void BeginPlay() override;

public:
	ACharacterRenderCapture2D* GetRenderCaptureActor();

	void UpdateMeshSegment(EHumanoidMeshSegment MeshSegment, USkeletalMesh* Mesh);

	void AttachActorToSocket(AActor* Actor, FName SocketName, FVector RelativeLocation, FRotator RelativeRotation);

	void DetachActor(AActor* Actor);
};
