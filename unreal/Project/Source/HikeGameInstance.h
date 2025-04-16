// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsEnv.h"
#include "Engine/GameInstance.h"
#include "HikeGameInstance.generated.h"


/**
 * 
 */
UCLASS()
class HIKE_API UHikeGameInstance : public UGameInstance
{
	GENERATED_BODY()

	// 调用ts函数的代理
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FTSCall, FString, FunctionName, UObject*, Uobject);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hike|Puerts")
	FString GameScriptEntry{"MainGame"};

public:
	virtual void Init() override;
	virtual void OnStart() override;
	virtual void Shutdown() override;

	UPROPERTY()
	FTSCall FCall;

	UFUNCTION(BlueprintCallable)
	void CallTS(FString FunctionName, UObject* Uobject);

protected:
	TWeakPtr<puerts::FJsEnv> GameScript;
};
