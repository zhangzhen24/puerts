/*
 * Tencent is pleased to support the open source community by making Puerts available.
 * Copyright (C) 2020 Tencent.  All rights reserved.
 * Puerts is licensed under the BSD 3-Clause License, except for the third-party components listed in the file 'LICENSE' which may
 * be subject to their corresponding license terms. This file is subject to the terms and conditions defined in file 'LICENSE',
 * which is part of this source code package.
 */

#include "AutoMixinListener.h"
#include "JsEnvImpl.h"
#include "IPuertsMixinInterface.h"
#include "Containers/Ticker.h"
#include "Async/Async.h"
#include "UObject/UObjectIterator.h"

// Define log category
DEFINE_LOG_CATEGORY(LogAutoMixin);

FAutoMixinListener& FAutoMixinListener::Get()
{
    static FAutoMixinListener Instance;
    return Instance;
}

void FAutoMixinListener::Initialize(PUERTS_NAMESPACE::FJsEnvImpl* InJsEnv)
{
    if (bIsInitialized)
    {
        return;
    }

    JsEnv = InJsEnv;

    GUObjectArray.AddUObjectCreateListener(this);
    GUObjectArray.AddUObjectDeleteListener(this);

    bIsInitialized = true;
}

void FAutoMixinListener::Shutdown()
{
    if (!bIsInitialized)
    {
        return;
    }

    GUObjectArray.RemoveUObjectCreateListener(this);
    GUObjectArray.RemoveUObjectDeleteListener(this);

    {
        FScopeLock Lock(&CacheLock);
        ProcessedClasses.Empty();
    }

    JsEnv = nullptr;
    bIsInitialized = false;
}

bool FAutoMixinListener::ImplementsMixinInterface(const UClass* Class) const
{
    return Class && Class->ImplementsInterface(UPuertsMixinInterface::StaticClass());
}

void FAutoMixinListener::NotifyUObjectCreated(const UObjectBase* Object, int32 Index)
{
    if (!Object || !JsEnv)
    {
        return;
    }

    const UObject* Obj = static_cast<const UObject*>(Object);
    UClass* Class = const_cast<UClass*>(Obj->GetClass());

    if (!ImplementsMixinInterface(Class))
    {
        return;
    }

    {
        FScopeLock Lock(&CacheLock);
        if (ProcessedClasses.Contains(Class))
        {
            return;
        }
        ProcessedClasses.Add(Class);
    }

    auto ExecuteMixin = [this, Class]()
    {
        if (!JsEnv)
        {
            return;
        }

        UObject* CDO = Class->GetDefaultObject();
        if (!CDO)
        {
            return;
        }

        FString ModulePath;
        if (Class->ImplementsInterface(UPuertsMixinInterface::StaticClass()))
        {
            ModulePath = IPuertsMixinInterface::Execute_GetMixinModulePath(CDO);
        }

        if (ModulePath.IsEmpty())
        {
            return;
        }

        bool bSuccess = JsEnv->ExecuteAutoMixin(Class, ModulePath);
        if (bSuccess)
        {
            UE_LOG(LogAutoMixin, Log, TEXT("Mixin: %s -> %s"), *ModulePath, *Class->GetName());
        }
    };

    if (IsInGameThread())
    {
        ExecuteMixin();
    }
    else
    {
        AsyncTask(ENamedThreads::GameThread, ExecuteMixin);
    }
}

void FAutoMixinListener::OnUObjectArrayShutdown()
{
    Shutdown();
}

void FAutoMixinListener::ScanExistingObjects()
{
    if (!JsEnv)
    {
        return;
    }

    int32 ProcessedCount = 0;

    for (TObjectIterator<UClass> It; It; ++It)
    {
        UClass* Class = *It;
        if (!Class)
        {
            continue;
        }

        if (!ImplementsMixinInterface(Class))
        {
            continue;
        }

        {
            FScopeLock Lock(&CacheLock);
            if (ProcessedClasses.Contains(Class))
            {
                continue;
            }
            ProcessedClasses.Add(Class);
        }

        UObject* CDO = Class->GetDefaultObject();
        if (!CDO)
        {
            continue;
        }

        FString ModulePath = IPuertsMixinInterface::Execute_GetMixinModulePath(CDO);
        if (ModulePath.IsEmpty())
        {
            continue;
        }

        bool bSuccess = JsEnv->ExecuteAutoMixin(Class, ModulePath);
        if (bSuccess)
        {
            ProcessedCount++;
            UE_LOG(LogAutoMixin, Log, TEXT("Mixin: %s -> %s"), *ModulePath, *Class->GetName());
        }
    }

    if (ProcessedCount > 0)
    {
        UE_LOG(LogAutoMixin, Log, TEXT("Scanned %d existing classes"), ProcessedCount);
    }
}

void FAutoMixinListener::NotifyUObjectDeleted(const UObjectBase* Object, int32 Index)
{
    if (!Object || !JsEnv)
    {
        return;
    }

    const UClass* Class = Cast<UClass>(static_cast<const UObject*>(Object));
    if (!Class)
    {
        return;
    }

    bool bWasProcessed = false;
    {
        FScopeLock Lock(&CacheLock);
        bWasProcessed = ProcessedClasses.Contains(const_cast<UClass*>(Class));
        if (bWasProcessed)
        {
            ProcessedClasses.Remove(const_cast<UClass*>(Class));
        }
    }

    if (!bWasProcessed)
    {
        return;
    }

    ExecuteUnmixin(const_cast<UClass*>(Class));
}

void FAutoMixinListener::ExecuteUnmixin(UClass* Class)
{
    if (!JsEnv || !Class)
    {
        return;
    }

    auto DoUnmixin = [this, Class]()
    {
        if (!JsEnv)
        {
            return;
        }

        bool bSuccess = JsEnv->ExecuteUnmixin(Class);
        if (bSuccess)
        {
            UE_LOG(LogAutoMixin, Log, TEXT("Unmixin: %s"), *Class->GetName());
        }
    };

    if (IsInGameThread())
    {
        DoUnmixin();
    }
    else
    {
        AsyncTask(ENamedThreads::GameThread, DoUnmixin);
    }
}
