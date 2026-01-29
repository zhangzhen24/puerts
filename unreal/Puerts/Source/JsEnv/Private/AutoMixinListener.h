/*
 * Tencent is pleased to support the open source community by making Puerts available.
 * Copyright (C) 2020 Tencent.  All rights reserved.
 * Puerts is licensed under the BSD 3-Clause License, except for the third-party components listed in the file 'LICENSE' which may
 * be subject to their corresponding license terms. This file is subject to the terms and conditions defined in file 'LICENSE',
 * which is part of this source code package.
 */

#pragma once

#include "CoreMinimal.h"
#include "PuertsNamespaceDef.h"
#include "UObject/UObjectArray.h"

// Declare log category for AutoMixin
DECLARE_LOG_CATEGORY_EXTERN(LogAutoMixin, Log, All);

namespace PUERTS_NAMESPACE
{
class FJsEnvImpl;
}

/**
 * Listens for UObject creation/deletion to implement Auto Mixin/Unmixin.
 * When a UObject is created that implements IPuertsMixinInterface,
 * it immediately processes the mixin on the game thread.
 * When a UClass is about to be destroyed, it unmixes the class.
 */
class JSENV_API FAutoMixinListener : public FUObjectArray::FUObjectCreateListener, public FUObjectArray::FUObjectDeleteListener
{
public:
    static FAutoMixinListener& Get();

    /** Initialize the listener with a JsEnv instance */
    void Initialize(PUERTS_NAMESPACE::FJsEnvImpl* InJsEnv);

    /** Shutdown the listener */
    void Shutdown();

    /** Check if initialized */
    bool IsInitialized() const { return bIsInitialized; }

    /** Scan existing objects that implement IPuertsMixinInterface */
    void ScanExistingObjects();

    // FUObjectCreateListener interface
    virtual void NotifyUObjectCreated(const UObjectBase* Object, int32 Index) override;
    virtual void OnUObjectArrayShutdown() override;

    // FUObjectDeleteListener interface
    virtual void NotifyUObjectDeleted(const UObjectBase* Object, int32 Index) override;

private:
    FAutoMixinListener() = default;
    virtual ~FAutoMixinListener() = default;

    // Non-copyable
    FAutoMixinListener(const FAutoMixinListener&) = delete;
    FAutoMixinListener& operator=(const FAutoMixinListener&) = delete;

    /** Check if a class implements IPuertsMixinInterface */
    bool ImplementsMixinInterface(const UClass* Class) const;

    /** Execute unmixin for a class */
    void ExecuteUnmixin(UClass* Class);

    /** JsEnv raw pointer (must be valid while listener is active) */
    PUERTS_NAMESPACE::FJsEnvImpl* JsEnv = nullptr;

    /** Whether initialized */
    bool bIsInitialized = false;

    /** Processed classes cache (avoid re-mixin same class) */
    TSet<TWeakObjectPtr<UClass>> ProcessedClasses;

    /** Cache lock */
    FCriticalSection CacheLock;
};
