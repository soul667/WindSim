// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

#include "Components/StaticMeshComponent.h" //�������
#include "Materials/MaterialInstanceConstant.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "WindContorlCom.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WINDSIM_API UWindContorlCom : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWindContorlCom();
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
	COLOR_GAME COLOR = COLOR_GAME::RED;
	//  1/3�� rad/s
	float speed = 1.0f / 3.0f * PI;
	AActor* SpinContorl;

	UMaterialInstanceConstant* TargetMaterial;
	UMaterialInstanceConstant* NormalMaterial;
	UMaterialInstanceConstant* Material[3]; // 0 1 2 �ֱ�������� ��ɫ ��ɫ
	//UMaterialInstanceConstant
	UStaticMeshComponent* WindMainPart1;
	UStaticMeshComponent* WindMainPart2;
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
		FAN_MODE MODE = FAN_MODE::ACTIVING; // Ĭ��ȫ�����Ǽ���ģʽ
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
	}WindState;
	URotatingMovementComponent* WindSpinContorl;
	APlayerController* PlayerController;
    UCameraComponent* Cam;
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
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
