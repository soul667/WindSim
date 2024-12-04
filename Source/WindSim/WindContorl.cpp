// Fill out your copyright notice in the Description page of Project Settings.


#include "WindContorl.h"

// Sets default values for this component's properties
UWindContorl::UWindContorl()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//UE_LOG(LogTemp, Warning, TEXT("C++ ENTER"));
	// ...
}


// Called when the game starts
void UWindContorl::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Warning, TEXT("C++ BEGIN"));

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("C++ Control Game Started"));
	GameInit();
	// ...
        // 输出到屏幕 用于测试
	// 输出到屏幕 用于测试
	

	
}


// Called every frame
void UWindContorl::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ChangeMode();
	Settings();
	// ...
}

int UWindContorl::GetStateBySpeed() {
	// 获取当前距离游戏开始的ms
	float ElapsedTime = GetWorld()->GetTimeSeconds();
	// 获取当前的状态
	int ModResult = static_cast<int>(ElapsedTime * 30) % 3;
	return ModResult;

}

void UWindContorl::GetFans() {
	// 获取所有的风扇
	for (AActor* Actor : FoundActors) {
		// 开始获取
		//if (Actor->GetActorLabel().Contains(TEXT("Fan"))) {
		if (Actor->Tags.IsEmpty()) continue; // 如果没有标签就跳过
		if (Actor->Tags[0].ToString().Contains(TEXT("Fan")) && Actor->GetAttachParentActor() == WindSpinActor) {
			//if (Actor->GetActorLabel().Contains(TEXT("Base"))) continue;
			FString Label = Actor->Tags[0].ToString();
			FString NumberPart = Label.RightChop(Label.Len() - 1);
			int32 Number = FCString::Atoi(*NumberPart);
			if (Number - 1 <= 4)Fans[Number - 1].fa = Actor;
			//UE_LOG(LogTemp, Display, TEXT("Fan %s Find Fine %d"), *Label, Number);
		}
	}
	for (AActor* Actor : FoundActors) {
		// 第二次遍历
			// 是子组件的放到对应的数组中
		if (Actor->Tags.IsEmpty()) continue; // 如果没有标签就跳过
		if (Actor->Tags[0].ToString().Contains(TEXT("FlowLight"))) {
			for (int i = 0; i <= 4; i++)
			{
				if (Actor->GetAttachParentActor() && Actor->GetAttachParentActor()->GetAttachParentActor() == Fans[i].fa) {
					Fans[i].FlowLights.Add(Actor);
					//UE_LOG(LogTemp, Warning, TEXT("11111111111111111111111111111%s"),Actor->GetActorLabel());
					//TestLogName(Actor, TEXT("FlowLight"));
				}
			}
		}
		else if (Actor->Tags[0].ToString().Contains(TEXT("ElseLight"))) {
			for (int i = 0; i <= 4; i++) {
				if (Actor->GetAttachParentActor() == Fans[i].fa) {
					Fans[i].ElseLight = Actor;
					//TestLogName(Actor, TEXT("ElseLight"));
				}
			}
		}
		else if (Actor->Tags[0].ToString().Contains(TEXT("TargetLight"))) {
			for (int i = 0; i <= 4; i++) {
				if (Actor->GetAttachParentActor() == Fans[i].fa) {
					Fans[i].TargetLight = Actor;
					//TestLogName(Actor, TEXT("TargetLight"));
				}
			}
		}
		else if (Actor->Tags[0].ToString().Contains(TEXT("ScoreLight"))) {
			for (int i = 0; i <= 4; i++) {
				if (Actor->GetAttachParentActor() && Actor->GetAttachParentActor()->GetAttachParentActor() == Fans[i].fa) {
					Fans[i].ScoreLights.Add(Actor);
					//TestLogName(Actor, TEXT("ScoreLight"));
				}
			}
		}
	}
	for (int i = 0; i <= 4; i++)
	{
		Fans[i].FlowLights.Sort([](const AActor& A, const AActor& B) {
			if (A.Tags.IsEmpty() || B.Tags.IsEmpty()) return false;
			return A.Tags[0].ToString() < B.Tags[0].ToString();
			});
		Fans[i].ScoreLights.Sort([](const AActor& A, const AActor& B) {
			if (A.Tags.IsEmpty() || B.Tags.IsEmpty()) return false;
			if (A.Tags[0].ToString().Contains(TEXT("ScoreLight10"))) return false;
			if (B.Tags[0].ToString().Contains(TEXT("ScoreLight10"))) return true;

			return A.Tags[0].ToString() < B.Tags[0].ToString();
			});
	}
	//for (auto& use_ : Fans[0].ScoreLights) {
 //       UE_LOG(LogTemp, Warning, TEXT("ScoreLights Actor: %s"), *use_->Tags[0].ToString());
	//}
	for (int i = 0; i <= 4; i++) {
		//	//Cast<UStaticMeshComponent>(use_->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		for (auto& j : Fans[i].FlowLights) {
			if (j) {
				UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(j->GetComponentByClass(UStaticMeshComponent::StaticClass()));
				if (StaticMeshComp) {
					Fans[i].FlowLight_.Add(StaticMeshComp);
					//UE_LOG(LogTemp, Warning, TEXT("StaticMeshComponent is OK for actor "));
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("StaticMeshComponent is null for actor %s"), j->Tags.IsEmpty() ? TEXT("NULL") : *j->Tags[0].ToString());
				}
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Actor in FlowLights is null"));
			}
		}
		if (Fans[i].TargetLight) {
			Fans[i].TargetLight_ = Cast<UStaticMeshComponent>(Fans[i].TargetLight->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		}
		else {
			Fans[i].TargetLight_ = nullptr;
			//LOG ERROR
			UE_LOG(LogTemp, Warning, TEXT("UStaticMeshComponent TargetLight is null"));


		}
		Fans[i].ElseLight_ = Fans[i].ElseLight ? Cast<UStaticMeshComponent>(Fans[i].ElseLight->GetComponentByClass(UStaticMeshComponent::StaticClass())) : nullptr;
		for (auto& j : Fans[i].ScoreLights) {
			if (j) {
				Fans[i].ScoreLights_.Add(Cast<UStaticMeshComponent>(j->GetComponentByClass(UStaticMeshComponent::StaticClass())));
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Actor in ScoreLights is null"));
			}
		}
	}
}
void UWindContorl::TestLogName(AActor* use_, FString use_string) {
	if (use_ && !use_->Tags.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("%s attached to %s"), *use_->Tags[0].ToString(), *use_string);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Invalid actor or label for %s attachment"), *use_string);
	}
}

void UWindContorl::ChangeMode() {
	// 获取当前时间 ms
	float ElapsedTime = GetWorld()->GetTimeSeconds();
	int TimeUse = static_cast<int>(ElapsedTime * 1000);

	if (ElapsedTime - WindState.LastHitTime > 2.5 || WindState.HitNum == 0) {
		WindState.LastHitTime = ElapsedTime; // 超过2S还未击打就更新下一片
		for (int i = 0; i <= 4; i++)
		{
			WindState.state[i] = FAN_MODE::NOACTIVATE;
			Fans[i].Score = 0;
			if (Fans[i].TargetLight) {
				Fans[i].TargetLight->SetActorHiddenInGame(false);
			}
		}
		// 生成一个随机数
		// 假如已经打中了x环 还剩下x-1环 那么
		WindState.HitNum = 0;
		int StartNum = FMath::RandRange(0, 5 - WindState.HitNum - 1); // 从第x个数出发后面的第一个可击打的扇叶
		int UseNum = 1;
		for (int i = 1; i <= UseNum; i++)
		{
			int j = (WindState.NowHit + i) % 5;
			if (WindState.state[j] == FAN_MODE::ACTIVED) UseNum = UseNum + 1;
			else {
				WindState.NowHit = j;
				break;
			}
			//while(WindState.)
		}
		//WindState.NowHit
		WindState.state[WindState.NowHit] = FAN_MODE::ACTIVING; // 正在激活状态
		WindState.HitNum = 1;
	}
	if (WindState.state[WindState.NowHit] == FAN_MODE::ACTIVED) {
		if (WindState.HitNum != 6)
		{
			WindState.LastHitTime = ElapsedTime;
			if (Fans[WindState.NowHit].fa == nullptr) return; // 如果没有风扇 直接退出
			for (auto& j : Fans[WindState.NowHit].FlowLight_) {
				if (j) {
					if (j && j->GetMaterial(0)) {
						j->SetMaterial(0, TargetMaterial);
					}
					// LOG SETTING FlowLight Right
				}
			}


			if (Fans[WindState.NowHit].TargetLight_) {
				Fans[WindState.NowHit].TargetLight_->SetMaterial(0, Material[0]);
				// 并且设置它的状态为隐藏
                // 设置Fans[WindState.NowHit].TargetLight的可见性  
                if (Fans[WindState.NowHit].TargetLight) {  
                   Fans[WindState.NowHit].TargetLight->SetActorHiddenInGame(true);  
                }
			}
			if (Fans[WindState.NowHit].ScoreLights_[Fans[WindState.NowHit].Score-1]) {
				Fans[WindState.NowHit].ScoreLights_[Fans[WindState.NowHit].Score-1]->SetMaterial(0, TargetMaterial);
			}
			if (Fans[WindState.NowHit].ElseLight_) {
				Fans[WindState.NowHit].ElseLight_->SetMaterial(0, TargetMaterial);
			}
	/*		for (auto& j : Fans[WindState.NowHit].ScoreLights_) {
				if (j) {
					j->SetMaterial(0, TargetMaterial);
				}
			}*/
			if (WindState.HitNum == 5) {
				WindState.HitNum = 6;
			};
		}
			
		if (WindState.HitNum!=6) {
			int StartNum = FMath::RandRange(0, 5 - WindState.HitNum - 1); // 从第x个数出发后面的第一个可击打的扇叶
			int UseNum = 1;
			for (int i = 1; i <= UseNum; i++)
			{
				int j = (WindState.NowHit + i) % 5;
				if (WindState.state[j] == FAN_MODE::ACTIVED) UseNum = UseNum + 1;
				else {
					WindState.NowHit = j;
					break;
				}
				//while(WindState.)
			}
			//WindState.NowHit
			WindState.state[WindState.NowHit] = FAN_MODE::ACTIVING; // 正在激活状态
			WindState.HitNum = WindState.HitNum + 1;
		}
	}
	WindRender();
}

void UWindContorl::Settings() {
	// 按键检测切换 模式显示
	float TimeNow = GetWorld()->GetTimeSeconds();
    if (PlayerController->IsInputKeyDown(EKeys::V)&& TimeNow - KeyState.LastSpinModeTime > 0.5)
    {
        KeyState.LastSpinModeTime = TimeNow; // 修复问题1: 添加表达式
        if (WindState.SpinMode == SPIN_MODE::SMALL) {
            GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("Change To Big Mode"));
            WindState.LastSpinMode = SPIN_MODE::SMALL;
            WindState.SpinMode = SPIN_MODE::BIG;
            WindState.a = FMath::RandRange(0.780f, 1.045f); //a 的取值范围为 0.780~1.045
            WindState.omega = FMath::RandRange(1.884f, 2.000f); //ω的取值范围为 1.884~2.000
			//：spd = a ∗ sin(𝜔𝜔 ∗ 𝑡𝑡) + 𝑏𝑏，其中 spd 的单位
				//为 rad / s，t 的单位为 s，a 的取值范围为 0.780~1.045，ω的取值范围为 1.884~2.000，b 始终满足 b = 2.090 -
				//a
        }
        else {
            GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("Change To Small Mode"));
            WindState.LastSpinMode = SPIN_MODE::BIG;
            WindState.SpinMode = SPIN_MODE::SMALL;
        }
    } // 修复问题2: 添加分号
	if (PlayerController->IsInputKeyDown(EKeys::C) && TimeNow - KeyState.LastColorModeTime > 0.5) {
		KeyState.LastColorModeTime = GetWorld()->GetTimeSeconds();
		if (WindState.Color == COLOR_GAME::RED) {
			GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("Change To Blue Mode"));
			WindState.Color = COLOR_GAME::BLUE;
			TargetMaterial = Material[2];
		}
		else {
			GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("Change To Red Mode"));
			WindState.Color = COLOR_GAME::RED;
			TargetMaterial = Material[1];

		}
		ColorChange = true;
	}
	int Score_ = 0;
	if (TimeNow - KeyState.LastActivateModeTime > 0.3) {
		if (PlayerController->IsInputKeyDown(EKeys::Zero)) Score_ = 10;
		else if (PlayerController->IsInputKeyDown(EKeys::Nine)) Score_ = 9;
		else if (PlayerController->IsInputKeyDown(EKeys::Eight)) Score_ = 8;
		else if (PlayerController->IsInputKeyDown(EKeys::Seven)) Score_ = 7;
		else if (PlayerController->IsInputKeyDown(EKeys::Six)) Score_ = 6;
		else if (PlayerController->IsInputKeyDown(EKeys::Five)) Score_ = 5;
		else if (PlayerController->IsInputKeyDown(EKeys::Four)) Score_ = 4;
		else if (PlayerController->IsInputKeyDown(EKeys::Three)) Score_ = 3;
		else if (PlayerController->IsInputKeyDown(EKeys::Two)) Score_ = 2;
		else if (PlayerController->IsInputKeyDown(EKeys::One)) Score_ = 1;
	}
	
	if (Score_) {
        GEngine->AddOnScreenDebugMessage(3, 8.f, FColor::White, FString::Printf(TEXT("Activate fan[%d]=%d NowHitNum=%d"), WindState.NowHit, Score_,WindState.HitNum));
		//WindState.HitNum += 1;
		WindState.AllScore +=Score_;
		Fans[WindState.NowHit].Score = Score_;
		Fans[WindState.NowHit].MODE = FAN_MODE::ACTIVED; // 已经被激活
		WindState.state[WindState.NowHit] = FAN_MODE::ACTIVED;
		KeyState.LastActivateModeTime = TimeNow;
		WindState.LastHitTime = TimeNow;
		WindState.LastState[WindState.NowHit] = FAN_MODE::ACTIVED;
	}

	if (PlayerController->IsInputKeyDown(EKeys::S) && PlayerController->IsInputKeyDown(EKeys::LeftControl) &&TimeNow- KeyState.LastSaveTime>0.5) {
            if (SceneCapture) {
				;
            }
		}
	// 设置能量机关的速度
	if (WindState.LastSpinMode == WindState.SpinMode && WindState.SpinMode == SPIN_MODE::BIG) return;
    FRotator Rotator = WindSpinContorl->RotationRate;
    if (WindState.SpinMode == SPIN_MODE::SMALL) {
        // 将yaw设置为60
        WindSpinContorl->RotationRate.Yaw = 60;
    } else {
        // 将yaw设置为120
		float angle = WindState.a * FMath::Sin(WindState.omega * GetWorld()->GetTimeSeconds()) + (2.090f - WindState.a);
		angle = FMath::RadiansToDegrees(angle);
        WindSpinContorl->RotationRate.Yaw = angle; // spd = a ∗ sin(𝜔𝜔 ∗ 𝑡𝑡) + 𝑏𝑏，其中 spd 的单位为 rad / s，
    }
}

// 游戏用到的所有的初始化
void UWindContorl::GameInit() {
	LoadMaterial();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);
	// 输出所有的ActorName
	for (AActor* Actor : FoundActors) {
		/*if (Actor->GetActorLabel().Equals(TEXT("FlowLight"))) {
			FlowLightActor = Actor;
		}*/
		//UE_LOG(LogTemp, Display, TEXT("%s"), *Actor->GetActorLabel());
		if (Actor->Tags.IsEmpty())continue;
		//UE_LOG(LogTemp, Display, TEXT("%s"), *Actor->Tags[0].ToString());

		if (Actor->Tags[0].ToString() == (TEXT("WindSpin-1"))) { // 注意是就等于
			WindSpinActor = Actor;
			//UE_LOG(LogTemp, Display, TEXT("SpinActor Find Fine"));
			(Actor->GetRootComponent())->SetMobility(EComponentMobility::Movable);
            WindSpinContorl = Cast<URotatingMovementComponent>(Actor->GetComponentByClass(URotatingMovementComponent::StaticClass()));
		}
		if (Actor->Tags[0].ToString().Contains(TEXT("GameCamMain"))) { // 注意是就等于
			UE_LOG(LogTemp, Display, TEXT("GameCamMainActor Find Fine"));
			Cam = Cast<UCameraComponent>(Actor->GetComponentByClass(UCameraComponent::StaticClass()));
			SceneCapture = Cast<USceneCaptureComponent2D>(Actor->GetComponentByClass(USceneCaptureComponent2D::StaticClass()));
			if (SceneCapture) {
				// 输出Cam的曝光  
			//	Cam->PostProcessSettings.expo
                UE_LOG(LogTemp, Display, TEXT("SceneCapture Fine"));
				// 保存图像
				//SceneCapture->CaptureScene();

			}
		}

		if (Actor->Tags[0].ToString().Contains(TEXT("SpotLight"))) { // 注意是就等于
			//UE_LOG(LogTemp, Display, TEXT("SpotLight %s Find"), *Actor->Tags[0].ToString());
			EnvironmentLights.Add(Cast<ULightComponent>(Actor->GetComponentByClass(ULightComponent::StaticClass())));
		}
		if (Actor->Tags[0].ToString().Contains(TEXT("MainPart1"))) {
			WindMainPart1 = Cast<UStaticMeshComponent>(Actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			if (WindMainPart1) {
				WindMainPart1->SetMaterial(0, Material[0]);
				//UE_LOG(LogTemp, Display, TEXT("MainPart1 Find Fine"));
			}

		}
		if (Actor->Tags[0].ToString().Contains(TEXT("MainPart2"))) {
			WindMainPart2 = Cast<UStaticMeshComponent>(Actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			if (WindMainPart2) {
				WindMainPart2->SetMaterial(0, Material[0]);
				//UE_LOG(LogTemp, Display, TEXT("MainPart2 Find Fine"));
			}
		}
		if (Actor->Tags[0].ToString().Contains(TEXT("WindSpin-1_R-1_RLight-1"))) {
			CenterLight_ = Cast<UStaticMeshComponent>(Actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			if (CenterLight_) {
				CenterLight_->SetMaterial(0, TargetMaterial);
				//UE_LOG(LogTemp, Display, TEXT("RLight Find Fine"));
			}

		}
	}
	GetFans();

	for (int i = 0; i <= 4; i++) {
		WindState.state[i] = FAN_MODE::NOACTIVATE;
		WindState.LastState[i] = FAN_MODE::ACTIVED;
		for (auto& j : Fans[i].FlowLight_) {
			if (j) {
				if (j && j->GetMaterial(0)) {
					j->SetMaterial(0, Material[0]);
					//UE_LOG(LogTemp, Display, TEXT("SET FlowLight Normal Right"));

				}

				// LOG SETTING FlowLight Right
			}
		}
		if (Fans[i].TargetLight_) {
			Fans[i].TargetLight_->SetMaterial(0, Material[0]);
		}
		if (Fans[i].ElseLight_) {
			Fans[i].ElseLight_->SetMaterial(0, Material[0]);
		}
		for (auto& j : Fans[i].ScoreLights_) {
			if (j) {
				j->SetMaterial(0, Material[0]);
			}
		}
	}
	// 获得我们的玩家控制器
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, TEXT("Play Get Ok"));
		// 允许输入
		//EnableInput(PlayerController);
        PlayerController->EnableInput(PlayerController);

	}
}
void UWindContorl::LoadMaterial() {
	Material[0] = LoadObject<UMaterialInstanceConstant>(
		nullptr,
		TEXT("/Game/WIND/Materials/color_1098367027_.color_1098367027_"),
		nullptr,
		LOAD_None,
		nullptr
	);
	/// Script / Engine.MaterialInstanceConstant'/Game/Wind/WIND/Materials/color_1098367027_.color_1098367027_'
	Material[1] = LoadObject<UMaterialInstanceConstant>(
		nullptr,
		TEXT("/Game/WIND/Materials/red_LED_1814320893_.red_LED_1814320893_"),
		nullptr,
		LOAD_None,
		nullptr
	);
	Material[2] = LoadObject<UMaterialInstanceConstant>(
		nullptr,
		TEXT("/Game/WIND/Materials/blue_led_1230032108_.blue_led_1230032108_"),
		nullptr,
		LOAD_None,
		nullptr
	);
	if (Material[0]) {
		TargetMaterial = Material[1];
	}
	/// Script / Engine.MaterialInstanceConstant'/Game/Wind/WIND/Materials/white_low_gloss_plastic_1767808287_.white_low_gloss_plastic_1767808287_'
}
/// Script / Engine.MaterialInstanceConstant''
void UWindContorl::WindRender() {
	// 根据状态渲染我们的能量机关
	//if(UMaterialInstanceConstant[0])
	for (int i = 0; i <= 2; i++) {
		if (Material[i] == nullptr) return; // 如果没有材料 直接退出
	}
	for (int i = 0; i <= 4; i++) {
		if (Fans[i].fa == nullptr) return; // 如果没有风扇 直接退出
		if (WindState.state[i] == WindState.LastState[i] && WindState.NowHit != i &&!ColorChange) continue; // 如果没有变化并且不需要流水就不渲染这一帧率
		if (WindState.state[i] == FAN_MODE::NOACTIVATE) {
			// 只有一种情况那就是原来是亮起状态 
			WindState.LastState[i] = FAN_MODE::NOACTIVATE;
			for (auto& j : Fans[i].FlowLight_) {
				if (j) {
					if (j && j->GetMaterial(0)) {
						j->SetMaterial(0, Material[0]);
						//UE_LOG(LogTemp, Display, TEXT("SET FlowLight Normal Right"));

					}

					// LOG SETTING FlowLight Right
				}
			}
			if (Fans[i].TargetLight_) {
				Fans[i].TargetLight_->SetMaterial(0, Material[0]);
			}
			if (Fans[i].ElseLight_) {
				Fans[i].ElseLight_->SetMaterial(0, Material[0]);
			}
			for (auto& j : Fans[i].ScoreLights_) {
				if (j) {
					j->SetMaterial(0, Material[0]);
				}
			}

		}
		else if (WindState.state[i] == FAN_MODE::ACTIVING)
		{

			float ElapsedTime = GetWorld()->GetTimeSeconds();
			int FlowUseState = static_cast<int>(ElapsedTime * 10) % 3;
			// 输出FlowUseState
			//UE_LOG(LogTemp, Display, TEXT("FlowUseState is %d"), FlowUseState);
			if (FlowUseState != WindState.LastFlowUseState || ColorChange) {
				WindState.LastFlowUseState = FlowUseState;
				for (auto& j : Fans[i].FlowLight_) {
					if (j) {
						if (j && j->GetMaterial(0)) {
							j->SetMaterial(0, Material[0]);
							//UE_LOG(LogTemp, Display, TEXT("SET FlowLight Red Right"));
						}
						// LOG SETTING FlowLight Right
					}
				}
				Fans[i].FlowLight_[FlowUseState + 1]->SetMaterial(0, TargetMaterial);
			}
			// 静态的亮起只更新一次
			if (WindState.LastState[i] != FAN_MODE::ACTIVING || ColorChange)
			{
				for (auto& j : Fans[i].ScoreLights_) {
					if (j) {
						j->SetMaterial(0, Material[0]);
					}
				}
				if (Fans[i].ElseLight_) {
					Fans[i].ElseLight_->SetMaterial(0, Material[0]);
				}
				// 记得更新颜色 Material[1]全局替换即可
				if (Fans[i].TargetLight_) {
					Fans[i].TargetLight_->SetMaterial(0, TargetMaterial);
				}
				WindState.LastState[i] = FAN_MODE::ACTIVING;
			}

		}

	}
	if (ColorChange) {
		ColorChange = false;
		CenterLight_->SetMaterial(0, TargetMaterial);
	}
	//for(int)
}