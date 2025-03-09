// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

#include "Components/StaticMeshComponent.h" //�������
#include "Materials/MaterialInstanceConstant.h"
#include "GameFramework/RotatingMovementComponent.h"


#include <InputCoreTypes.h>
#include "GameFramework/PlayerController.h"

#include "Components/LightComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
// ��ͼ

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
	float flow_speed = 0.0f; // �������ٶ�
	enum class FAN_MODE {
		NOACTIVATE,
		ACTIVED,
		ACTIVING,
	};
	TArray<AActor*> FoundActors;
	TArray<AActor*> UseActors;//FlowLightActor����Actor ��ǰ��ˮ�Ƶ�Actor
	AActor* FlowLightActor;
	AActor* WindSpinActor;
	FAN_MODE MODE = FAN_MODE::ACTIVING;
	//  1/3�� rad/s
	float speed = 1.0f / 3.0f * PI;
	AActor* SpinContorl;

	UMaterialInstanceConstant* TargetMaterial;
	UMaterialInstanceConstant* Material[3]; // 0 1 2 �ֱ�������� ��ɫ ��ɫ
	//UMaterialInstanceConstant
	UStaticMeshComponent* WindMainPart1;
	UStaticMeshComponent* WindMainPart2;
	UStaticMeshComponent* CenterLight_;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	AActor* CenterActorUse;
	TArray<ULightComponent*> EnvironmentLights;// �����ƹ� 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	AActor* CamActor;
	UCameraComponent* Cam;
    //#include "Components/PointLightComponent.h"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2D Render")
    USceneCaptureComponent2D* SceneCapture;
    //UPointLightComponent* PointLight;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveImage")
    FString ExportPath = TEXT("E:/RM/WindExport");

	bool ColorChange = false;// �������仯�� ����2sҲ������ɨ��
	struct Fan {
		AActor* fa;// ��Ҷ�����Actor
		//UStaticMeshComponent* fa_;// ��Ҷ��������
		TArray<AActor*> FlowLights;// ��Ҷ�ϵ���ˮ�� ������ 0 1 2 3 �ֱ��ʾ�ĸ���ͬ���
		TArray<UStaticMeshComponent*> FlowLight_;
		AActor* TargetLight; // �б�;
		UStaticMeshComponent* TargetLight_;
		AActor* ElseLight;// ������Ƭ
		UStaticMeshComponent* ElseLight_;
		TArray<AActor*> ScoreLights;// �÷ֵ�Ƭ ��0��9�ֱ�Ϊ 1�� 2�� 3�� 4�� 5�� 6�� 7�� 8�� 9�� 10��
		TArray<UStaticMeshComponent*> ScoreLights_;
		//AActor* CenterLight;
		FAN_MODE MODE = FAN_MODE::ACTIVING; // Ĭ��ȫ�����Ǽ���ģʽ
		int Score = 0;// �����еĻ���
		bool AlreadyHit = 0;
		// Cast<UStaticMeshComponent>(use_->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	};
	//Fan Fans
	// ����fans����
	//TArray<Fan> Fans;
	Fan Fans[5];
	enum SPIN_MODE {
		SMALL = 0,// 60��ÿ��
		BIG = 1,// ������Ҳ�
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
	}KeyState; //������ʱ������ֹһֱ�����л�ģʽ
	struct State {
		int speed = 0;
		SPIN_MODE SpinMode = SPIN_MODE::SMALL;
		SPIN_MODE LastSpinMode = SPIN_MODE::SMALL;
		int8 NowHit = 0; // ������Ҫ�������Ƭ��Ҷ
		int HitState; // ����״̬
		int HitNum = 0;// �Ѿ�������Ҷ������
		FAN_MODE state[5]; // 0 1 2 3 4 �ֱ���������Ҷ��״̬
		FAN_MODE LastState[5];
		int HitHuan = 0;//���еĻ��� �����0������δ����
		float LastHitTime = 0;// �ϴλ����ʱ��
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

	TArray<FVector> RPoints;// ���ڴ洢����R�����Ϣ
	TArray<FVector2D> FanPoints;
	TArray<FVector> OutPoints; // �洢��������Ϣ

	TArray<FVector2D> WindPoints;

	TArray<KeyArea> Areas;
	TArray<Point> KeyPoints;	
	FTextureRenderTargetResource *TextureRenderTargetResource;
	// ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CamMove")
	bool CanMove = true; // �������Ƿ�����ƶ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveImage")
	int SavedImageNum = 0; //�ѱ����ͼ����Ŀ

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveImage")
	bool AutoSave = 0; //�Զ����� Ctrl+A����
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveImage")
	float AutoSaverTime = 0.5;
	
	// ģʽ����ʱ��仯����������̶� 
	bool AutoHit = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CamCopy")
	int CamNum = 0; // �Ѿ����������
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
	void ChangeMode();// �漰���任ģʽ ������
	void Settings();  // ��ݼ��任ģʽ
	void LoadMaterial();
	void GameInit();
	void WindRender();
	bool ProjectWorldToSceneCapture(USceneCaptureComponent2D* SceneCaptureComponent2D, const FVector& WorldPosition, FVector2D& ScreenPosition);

	//inline void CheckNumber();
	void ColorToImage(const FString& InImagePath, TArray<FColor> InColor, int32 InWidth, int32 InHight);
	FString GerImgName(FString Path, FString Name);
	bool WriteData(FString Path, FString Name);
	TArray<FVector2D> WorldToScreen(TArray<FVector>InputWorldPoints);
	bool InitCamMatrix();// ������λ�÷����ı����Ҫ���¼����������
	bool WorldPointsInit(); // ����Yaml�ļ�
	FVector2d ConverToImgSize(FVector2d UseVec);
	bool WorldPointsConvertToSceen(FVector FanCenter_, USceneCaptureComponent2D* SceneCapture);
	bool ProjectWorldTo2DSceneCapture(USceneCaptureComponent2D* SceneCaptureComponent2D, const FVector& WorldPosition, FVector2D& ScreenPosition);

	FVector InitLocation;
	FRotator InitRotation;
	bool RandomHit = 1; // �������
	FVector2D FanCenterUse2D;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
