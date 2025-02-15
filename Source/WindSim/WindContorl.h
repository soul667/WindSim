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
#include "Components/SceneCaptureComponent2D.h"
// 截图

#include "Components/SceneCaptureComponent2D.h" // Add this include statement
#include "Engine/TextureRenderTarget2D.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
//#include "FileHelper.h"
//#include "Paths.h"
#include "Engine/Texture2D.h"
#include "Misc/FileHelper.h"
#include "ImageUtils.h" 

//#include "opencv2/opencv.hpp"
 #include "WindContorl.generated.h"

//#include "CameraUtility.h"                                                                                                                         
//#include "Camera/CameraActor.h"
// Opencv
// #if WITH_OPENCV

// #endif

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
//UCLASS()

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	AActor* CenterActorUse;
	TArray<ULightComponent*> EnvironmentLights;// 环境灯光 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	AActor* CamActor;
	UCameraComponent* Cam;
    //#include "Components/PointLightComponent.h"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2D Render")
    USceneCaptureComponent2D* SceneCapture;
    //UPointLightComponent* PointLight;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveImage")
    FString ExportPath = TEXT("E:/RM/WindExport");

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
		bool AlreadyHit = 0;
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
		float LastCanUseTime = 0;
		float LastChangeCamLocationTime=0;
		float SaveModeTime=0;
		float LastRandomHitTime=0;
		float LastChangeColorTime = 0;
		bool bWasInputActive = false;
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
		int LastSetWinId = -1;
	}WindState;
	URotatingMovementComponent* WindSpinContorl;
	APlayerController* PlayerController;
	struct Point {
		FVector2D p;
		FString PointName; //ID
	};
	struct KeyArea {
		FVector2D LeftTop;
		FVector2D RightBottom;
		FString AreaName; //ID
	};

	TArray<FVector> RPoints;// 用于存储中心R标的信息
	TArray<FVector2D> FanPoints;
	TArray<FVector> OutPoints; // 存储外面点的信息

	TArray<FVector2D> WindPoints;

	TArray<KeyArea> Areas;
	TArray<Point> KeyPoints;	
	FTextureRenderTargetResource *TextureRenderTargetResource;
	// 相机组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CamMove")
	bool CanMove = true; // 相机组件是否可以移动
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveImage")
	int SavedImageNum = 0; //已保存的图像数目

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveImage")
	bool AutoSave = 0; //自动保存 Ctrl+A开启
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveImage")
	float AutoSaverTime = 0.5;
	
	// 模式不随时间变化，根据输入固定 
	bool AutoHit = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CamCopy")
	int CamNum = 0; // 已经创建的相机
	bool MultiCamSave=1;
	TArray<AActor*>CamActors;	
	//struct KeyState {
	//}KeyState_;
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
	bool ProjectWorldToSceneCapture(USceneCaptureComponent2D* SceneCaptureComponent2D, const FVector& WorldPosition, FVector2D& ScreenPosition);

	//inline void CheckNumber();
	void ColorToImage(const FString& InImagePath, TArray<FColor> InColor, int32 InWidth, int32 InHight);
	FString GerImgName(FString Path, FString Name);
	bool WriteData(FString Path, FString Name);
	TArray<FVector2D> WorldToScreen(TArray<FVector>InputWorldPoints);
	bool InitCamMatrix();// 如果相机位置发生改变就需要重新计算相机矩阵
	bool WorldPointsInit(); // 解析Yaml文件
	FVector2d ConverToImgSize(FVector2d UseVec);
	bool WorldPointsConvertToSceen(FVector FanCenter_, USceneCaptureComponent2D* SceneCapture);
	bool ProjectWorldTo2DSceneCapture(USceneCaptureComponent2D* SceneCaptureComponent2D, const FVector& WorldPosition, FVector2D& ScreenPosition);

	FVector InitLocation;
	FRotator InitRotation;
	bool RandomHit = 1; // 随机按下
	FVector2D FanCenterUse2D;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
