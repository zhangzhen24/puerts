// Fill out your copyright notice in the Description page of Project Settings.


#include "HikeGameInstance.h"

#include "LogHikeChannels.h"
#include "PuertsModule.h"

void UHikeGameInstance::Init()
{
	Super::Init();
}

void UHikeGameInstance::OnStart()
{
	Super::OnStart();

	if (const IPuertsModule* Module = FModuleManager::GetModulePtr<IPuertsModule>("Puerts"))
	{
		GameScript = Module->GetJsEnv();
		if (GameScript.IsValid())
		{
			TArray<TPair<FString, UObject*>> Arguments;
			Arguments.Add(TPair<FString, UObject*>(TEXT("GameInstance"), this));
			GameScript.Pin()->Start(GameScriptEntry, Arguments);
			LOG_HIKE_ARGS(Log,"[Puerts] Loading Script {%s} Success!!", *GameScriptEntry);
		}
	}
}

void UHikeGameInstance::Shutdown()
{
	Super::Shutdown();

	GameScript.Reset();
}

void UHikeGameInstance::CallTS(FString FunctionName, UObject* Object)
{
	FCall.ExecuteIfBound(FunctionName, Object);
}
