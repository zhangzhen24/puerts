// PuertsMixin Interface - Blueprints implement this to support TypeScript Mixin auto-binding
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPuertsMixinInterface.generated.h"

UINTERFACE(BlueprintType, MinimalAPI)
class UPuertsMixinInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Blueprints implement this interface to support Auto Mixin.
 *
 * Usage:
 * 1. Blueprint implements IPuertsMixinInterface
 * 2. Override GetMixinModulePath() to return the TypeScript module path
 * 3. When an instance of this blueprint is created, the mixin is automatically applied
 */
class JSENV_API IPuertsMixinInterface
{
	GENERATED_BODY()

public:
	/**
	 * Returns the TypeScript module path to bind.
	 * Example: "Game/Actors/MyActor" corresponds to TypeScript/Game/Actors/MyActor.ts
	 *
	 * @return Module path string, empty string means no binding
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Puerts|Mixin")
	FString GetMixinModulePath() const;

	/** Default implementation returns empty string (no binding) */
	virtual FString GetMixinModulePath_Implementation() const { return FString(); }
};
