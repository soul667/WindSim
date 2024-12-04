// Fill out your copyright notice in the Description page of Project Settings.


#include "WindContorlCom.h"

// Sets default values for this component's properties
UWindContorlCom::UWindContorlCom()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWindContorlCom::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, TEXT("Control Com Game Started"));

	GameInit();

	// ...
	
}


// Called every frame
void UWindContorlCom::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ChangeMode();

	// ...
}

int UWindContorlCom::GetStateBySpeed() {
	// 获取当前距离游戏开始的ms
	float ElapsedTime = GetWorld()->GetTimeSeconds();
	// 获取当前的状态
	int ModResult = static_cast<int>(ElapsedTime * 30) % 3;
	return ModResult;

}

void UWindContorlCom::GetFans() {
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
			UE_LOG(LogTemp, Display, TEXT("Fan %s Find Fine %d"), *Label, Number);
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
					TestLogName(Actor, TEXT("FlowLight"));
				}
			}
		}
		else if (Actor->Tags[0].ToString().Contains(TEXT("ElseLight"))) {
			for (int i = 0; i <= 4; i++) {
				if (Actor->GetAttachParentActor() == Fans[i].fa) {
					Fans[i].ElseLight = Actor;
					TestLogName(Actor, TEXT("ElseLight"));
				}
			}
		}
		else if (Actor->Tags[0].ToString().Contains(TEXT("TargetLight"))) {
			for (int i = 0; i <= 4; i++) {
				if (Actor->GetAttachParentActor() == Fans[i].fa) {
					Fans[i].TargetLight = Actor;
					TestLogName(Actor, TEXT("TargetLight"));
				}
			}
		}
		else if (Actor->Tags[0].ToString().Contains(TEXT("ScoreLight"))) {
			for (int i = 0; i <= 4; i++) {
				if (Actor->GetAttachParentActor() && Actor->GetAttachParentActor()->GetAttachParentActor() == Fans[i].fa) {
					Fans[i].ScoreLights.Add(Actor);
					TestLogName(Actor, TEXT("ScoreLight"));
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
			return A.Tags[0].ToString() < B.Tags[0].ToString();
			});
	}
	//for (auto& use_ : Fans[0].FlowLights) {
	//	UE_LOG(LogTemp, Warning, TEXT("FlowLight Actor: %s"), *use_->GetActorLabel());
	//}
	for (int i = 0; i <= 4; i++) {
		//	//Cast<UStaticMeshComponent>(use_->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		for (auto& j : Fans[i].FlowLights) {
			if (j) {
				UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(j->GetComponentByClass(UStaticMeshComponent::StaticClass()));
				if (StaticMeshComp) {
					Fans[i].FlowLight_.Add(StaticMeshComp);
					UE_LOG(LogTemp, Warning, TEXT("StaticMeshComponent is OK for actor "));
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
void UWindContorlCom::TestLogName(AActor* use_, FString use_string) {
	if (use_ && !use_->Tags.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("%s attached to %s"), *use_->Tags[0].ToString(), *use_string);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Invalid actor or label for %s attachment"), *use_string);
	}
}

void UWindContorlCom::ChangeMode() {
	// 获取当前时间 ms
	float ElapsedTime = GetWorld()->GetTimeSeconds();
	int TimeUse = static_cast<int>(ElapsedTime * 1000);

	if (ElapsedTime - WindState.LastHitTime > 2 || WindState.HitNum == 0) {
		WindState.LastHitTime = ElapsedTime; // 超过2S就更新下一片
		for (int i = 0; i <= 4; i++)
		{
			WindState.state[i] = FAN_MODE::NOACTIVATE;
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
	// 输出WindState.NowHit
	//UE_LOG(LogTemp, Display, TEXT("NowHit is %d"), WindState.NowHit);
	WindRender();
}

void UWindContorlCom::Settings() {
	// 获取当前时间 ms
	;
	// 按键检测切换 模式显示
}

// 游戏用到的所有的初始化
void UWindContorlCom::GameInit() {
	LoadMaterial();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);
	// 输出所有的ActorName
	for (AActor* Actor : FoundActors) {
		/*if (Actor->GetActorLabel().Equals(TEXT("FlowLight"))) {
			FlowLightActor = Actor;
		}*/
		if (Actor->Tags.IsEmpty())continue;
		if (Actor->Tags[0].ToString() == (TEXT("WindSpin-1"))) { // 注意是就等于
			WindSpinActor = Actor;
			UE_LOG(LogTemp, Display, TEXT("SpinActor Find Fine"));
			(Actor->GetRootComponent())->SetMobility(EComponentMobility::Movable);
			WindSpinContorl = Cast<URotatingMovementComponent>(Actor->GetComponentByClass(URotatingMovementComponent::StaticClass()));
			if (WindSpinContorl) {
				// 输出旋转速率
				UE_LOG(LogTemp, Display, TEXT("RotationRate is %s"), *WindSpinContorl->RotationRate.ToString());

			}
			else {
				UE_LOG(LogTemp, Display, TEXT("WindSpinContorl is null"));
			}
			//输出Pictch
		}

		if (Actor->Tags[0].ToString().Contains(TEXT("MainPart1"))) {
			WindMainPart1 = Cast<UStaticMeshComponent>(Actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			if (WindMainPart1) {
				WindMainPart1->SetMaterial(0, Material[0]);
				UE_LOG(LogTemp, Display, TEXT("MainPart1 Find Fine"));
			}

		}
		if (Actor->Tags[0].ToString().Contains(TEXT("MainPart2"))) {
			WindMainPart2 = Cast<UStaticMeshComponent>(Actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			if (WindMainPart2) {
				WindMainPart2->SetMaterial(0, Material[0]);
				UE_LOG(LogTemp, Display, TEXT("MainPart2 Find Fine"));
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


}
void UWindContorlCom::LoadMaterial() {
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
	/// Script / Engine.MaterialInstanceConstant'/Game/Wind/WIND/Materials/white_low_gloss_plastic_1767808287_.white_low_gloss_plastic_1767808287_'
}
/// Script / Engine.MaterialInstanceConstant''
void UWindContorlCom::WindRender() {
	// 根据状态渲染我们的能量机关
	//if(UMaterialInstanceConstant[0])
	for (int i = 0; i <= 2; i++) {
		if (Material[i] == nullptr) return; // 如果没有材料 直接退出
	}
	for (int i = 0; i <= 4; i++) {
		if (Fans[i].fa == nullptr) return; // 如果没有风扇 直接退出
		if (WindState.state[i] == WindState.LastState[i] && WindState.NowHit != i) continue; // 如果没有变化并且不需要流水就不渲染这一帧率
		if (WindState.state[i] == FAN_MODE::NOACTIVATE) {
			// 只有一种情况那就是原来是亮起状态 
			WindState.LastState[i] = FAN_MODE::NOACTIVATE;
			for (auto& j : Fans[i].FlowLight_) {
				if (j) {
					if (j && j->GetMaterial(0)) {
						j->SetMaterial(0, Material[0]);
						UE_LOG(LogTemp, Display, TEXT("SET FlowLight Normal Right"));

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
			if (FlowUseState != WindState.LastFlowUseState) {
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
				Fans[i].FlowLight_[FlowUseState + 1]->SetMaterial(0, Material[1]);
			}
			// 静态的亮起只更新一次
			if (WindState.LastState[i] != FAN_MODE::ACTIVING)
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
					Fans[i].TargetLight_->SetMaterial(0, Material[1]);
				}
				WindState.LastState[i] = FAN_MODE::ACTIVING;
			}

		}

	}
	//for(int)
}