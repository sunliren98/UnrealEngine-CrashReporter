// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "LODActor.generated.h"

/**
 * LODActor is an instance of an autogenerated StaticMesh Actors by Hierarchical LOD System
 * This is essentially just StaticMeshActor that you can't move or edit, but it contains multiple actors reference
 *
 * @see https://docs.unrealengine.com/latest/INT/Engine/Actors/LODActor/
 * @see UStaticMesh
 */
UCLASS(notplaceable)
class ENGINE_API ALODActor : public AActor
{
	GENERATED_UCLASS_BODY()

private_subobject:
	// disable display of this component
	UPROPERTY(/*Category=LODActor, VisibleAnywhere*/)
	class UStaticMeshComponent* StaticMeshComponent;

public:
	UPROPERTY(Category=LODActor, VisibleAnywhere)
	TArray<class AActor*> SubActors;

	/** what distance do you want this to show up instead of SubActors */
	UPROPERTY(Category=LODActor, VisibleAnywhere)
	float LODDrawDistance;

	/** what distance do you want this to show up instead of SubActors */
	UPROPERTY(Category=LODActor, VisibleAnywhere)
	int32 LODLevel;

	/** assets that were created for this, so that we can delete them */
	UPROPERTY(Category=LODActor, VisibleAnywhere)
	TArray<class UObject*> SubObjects;

#if WITH_EDITOR
	// Begin AActor Interface
	virtual void CheckForErrors() override;
	virtual bool GetReferencedContentObjects( TArray<UObject*>& Objects ) const override;
	// End AActor Interface
#endif // WITH_EDITOR	

protected:
	// Begin UObject interface.
	virtual FString GetDetailedInfoInternal() const override;
	virtual FBox GetComponentsBoundingBox(bool bNonColliding = false) const override;	
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR	
	virtual void PostRegisterAllComponents() override;
	// End UObject interface.

public:
	/** Returns StaticMeshComponent subobject **/
	class UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }
};



