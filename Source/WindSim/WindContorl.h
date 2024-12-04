// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

#include "Components/StaticMeshComponent.h" //材质相关
#include "Materials/MaterialInstanceConstant.h"
#include "GameFramework/RotatingMovementComponent.h"


#include <InputCoreTypes.h>
#include "GameFramework/PlayerController.h"

#include "Components/LightComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h" // Add this include statement

#include "WindContorl.generated.h"


UCLASS( ClassGroup=(Wind), meta=(BlueprintSpawnableComponent) )
class WINDSIM_API UWindContorl : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWindContorl();
	enum class COLOR_GAME {
		RED,
		BLUE
	};
	float flow_speed = 0.0f; // 流动的速度
	enum class FAN_MODE {
		NOACTIVATE,
		ACTIVED,
		ACTIVING,
	};
	TArray<AActor*> FoundActors;
	TArray<AActor*> UseActors;//FlowLightActor的子Actor 当前流水灯的Actor
	AActor* FlowLightActor;
	AActor* WindSpinActor;
	FAN_MODE MODE = FAN_MODE::ACTIVING;
	
	//  1/3π rad/s
	float speed = 1.0f / 3.0f * PI;
	AActor* SpinContorl;

	UMaterialInstanceConstant* TargetMaterial;
	UMaterialInstanceConstant* Material[3]; // 0 1 2 分别代表正常 红色 蓝色
	//UMaterialInstanceConstant
	UStaticMeshComponent* WindMainPart1;
	UStaticMeshComponent* WindMainPart2;
	UStaticMeshComponent* CenterLight_;

	TArray<ULightComponent*> EnvironmentLights;// 环境灯光 
	UCameraComponent* Cam;
    //#include "Components/PointLightComponent.h"

    USceneCaptureComponent2D* SceneCapture;
    //UPointLightComponent* PointLight;
	bool ColorChange = false;// 如果这个变化了 不到2s也得重新扫描
	struct Fan {
		AActor* fa;// 扇叶本身的Actor
		//UStaticMeshComponent* fa_;// 扇叶本身的组件
		TArray<AActor*> FlowLights;// 扇叶上的流水灯 数组中 0 1 2 3 分别表示四个不同组件
		TArray<UStaticMeshComponent*> FlowLight_;
		AActor* TargetLight; // 靶标;
		UStaticMeshComponent* TargetLight_;
		AActor* ElseLight;// 两个灯片
		UStaticMeshComponent* ElseLight_;
		TArray<AActor*> ScoreLights;// 得分灯片 从0到9分别为 1分 2分 3分 4分 5分 6分 7分 8分 9分 10分
		TArray<UStaticMeshComponent*> ScoreLights_;
		//AActor* CenterLight;
		FAN_MODE MODE = FAN_MODE::ACTIVING; // 默认全部都是激活模式
		int Score = 0;// 击打中的环数
		// Cast<UStaticMeshComponent>(use_->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	};
	//Fan Fans
	// 定义fans数组
	//TArray<Fan> Fans;
	Fan Fans[5];
	enum SPIN_MODE {
		SMALL = 0,// 60度每秒
		BIG = 1,// 随机正弦波
	};

	struct KeyUse {
		float LastSpinModeTime=0;
		float LastColorModeTime = 0;
		float LastActivateModeTime = 0;
		float LastSaveTime = 0;

	}KeyState; //加入延时按键防止一直来回切换模式
	struct State {
		int speed = 0;
		SPIN_MODE SpinMode = SPIN_MODE::SMALL;
		SPIN_MODE LastSpinMode = SPIN_MODE::SMALL;
		int8 NowHit = 0; // 现在需要击打的那片扇叶
		int HitState; // 击打状态
		int HitNum = 0;// 已经激活扇叶的数量
		FAN_MODE state[5]; // 0 1 2 3 4 分别代表五个扇叶的状态
		FAN_MODE LastState[5];
		int HitHuan = 0;//打中的环数 如果是0环就是未打中
		float LastHitTime = 0;// 上次击打的时间
		int LastFlowUseState = 0;
		float a = 0.8;
		float omega = 1.9;
		int AllScore = 0;
		COLOR_GAME Color = COLOR_GAME::RED;
		float LastWindFiveTime = 0;
	}WindState;
	URotatingMovementComponent* WindSpinContorl;
	APlayerController* PlayerController;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	int GetStateBySpeed();
	void GetFans();
	void TestLogName(AActor* use_, FString use_string);
	void ChangeMode();// 涉及到变换模式 检测击打
	void Settings();  // 快捷键变换模式
	void LoadMaterial();
	void GameInit();
	void WindRender();
	//inline void CheckNumber();
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
