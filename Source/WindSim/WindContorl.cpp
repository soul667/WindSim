// Fill out your copyright notice in the Description page of Project Settings.

#include "WindContorl.h"
UWindContorl::UWindContorl()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UWindContorl::BeginPlay()
{
	Super::BeginPlay();
	GameInit();
}

// Called every frame
void UWindContorl::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ChangeMode();
	Settings();
}

int UWindContorl::GetStateBySpeed()
{
	// 获取当前距离游戏开始的ms
	float ElapsedTime = GetWorld()->GetTimeSeconds();
	// 获取当前的状态
	int ModResult = static_cast<int>(ElapsedTime * 30) % 3;
	return ModResult;
}

void UWindContorl::GetFans()
{
	// 获取所有的风扇
	for (AActor *Actor : FoundActors)
	{
		// 开始获取
		// if (Actor->GetActorLabel().Contains(TEXT("Fan"))) {
		if (Actor->Tags.IsEmpty())
			continue; // 如果没有标签就跳过
		if (Actor->Tags[0].ToString().Contains(TEXT("Fan")) && Actor->GetAttachParentActor() == WindSpinActor)
		{
			// if (Actor->GetActorLabel().Contains(TEXT("Base"))) continue;
			FString Label = Actor->Tags[0].ToString();
			FString NumberPart = Label.RightChop(Label.Len() - 1);
			int32 Number = FCString::Atoi(*NumberPart);
			if (Number - 1 <= 4)
				Fans[Number - 1].fa = Actor;
			// UE_LOG(LogTemp, Display, TEXT("Fan %s Find Fine %d"), *Label, Number);
		}
	}
	for (AActor *Actor : FoundActors)
	{
		// 第二次遍历
		// 是子组件的放到对应的数组中
		if (Actor->Tags.IsEmpty())
			continue; // 如果没有标签就跳过
		if (Actor->Tags[0].ToString().Contains(TEXT("FlowLight")))
		{
			for (int i = 0; i <= 4; i++)
			{
				if (Actor->GetAttachParentActor() && Actor->GetAttachParentActor()->GetAttachParentActor() == Fans[i].fa)
				{
					Fans[i].FlowLights.Add(Actor);
					// UE_LOG(LogTemp, Warning, TEXT("11111111111111111111111111111%s"),Actor->GetActorLabel());
					// TestLogName(Actor, TEXT("FlowLight"));
				}
			}
		}
		else if (Actor->Tags[0].ToString().Contains(TEXT("ElseLight")))
		{
			for (int i = 0; i <= 4; i++)
			{
				if (Actor->GetAttachParentActor() == Fans[i].fa)
				{
					Fans[i].ElseLight = Actor;
					// TestLogName(Actor, TEXT("ElseLight"));
				}
			}
		}
		else if (Actor->Tags[0].ToString().Contains(TEXT("TargetLight")))
		{
			for (int i = 0; i <= 4; i++)
			{
				if (Actor->GetAttachParentActor() == Fans[i].fa)
				{
					Fans[i].TargetLight = Actor;
					Actor->SetActorHiddenInGame(false);

					// TestLogName(Actor, TEXT("TargetLight"));
				}
			}
		}
		else if (Actor->Tags[0].ToString().Contains(TEXT("ScoreLight")))
		{
			for (int i = 0; i <= 4; i++)
			{
				if (Actor->GetAttachParentActor() && Actor->GetAttachParentActor()->GetAttachParentActor() == Fans[i].fa)
				{
					Fans[i].ScoreLights.Add(Actor);
					// TestLogName(Actor, TEXT("ScoreLight"));
				}
			}
		}
	}
	for (int i = 0; i <= 4; i++)
	{
		Fans[i].FlowLights.Sort([](const AActor &A, const AActor &B)
								{
			if (A.Tags.IsEmpty() || B.Tags.IsEmpty()) return false;
			return A.Tags[0].ToString() < B.Tags[0].ToString(); });
		Fans[i].ScoreLights.Sort([](const AActor &A, const AActor &B)
								 {
			if (A.Tags.IsEmpty() || B.Tags.IsEmpty()) return false;
			if (A.Tags[0].ToString().Contains(TEXT("ScoreLight10"))) return false;
			if (B.Tags[0].ToString().Contains(TEXT("ScoreLight10"))) return true;

			return A.Tags[0].ToString() < B.Tags[0].ToString(); });
	}
	// for (auto& use_ : Fans[0].ScoreLights) {
	//       UE_LOG(LogTemp, Warning, TEXT("ScoreLights Actor: %s"), *use_->Tags[0].ToString());
	//}
	for (int i = 0; i <= 4; i++)
	{
		//	//Cast<UStaticMeshComponent>(use_->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		for (auto &j : Fans[i].FlowLights)
		{
			if (j)
			{
				UStaticMeshComponent *StaticMeshComp = Cast<UStaticMeshComponent>(j->GetComponentByClass(UStaticMeshComponent::StaticClass()));
				if (StaticMeshComp)
				{
					Fans[i].FlowLight_.Add(StaticMeshComp);
					// UE_LOG(LogTemp, Warning, TEXT("StaticMeshComponent is OK for actor "));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("StaticMeshComponent is null for actor %s"), j->Tags.IsEmpty() ? TEXT("NULL") : *j->Tags[0].ToString());
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Actor in FlowLights is null"));
			}
		}
		if (Fans[i].TargetLight)
		{
			Fans[i].TargetLight_ = Cast<UStaticMeshComponent>(Fans[i].TargetLight->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		}
		else
		{
			Fans[i].TargetLight_ = nullptr;
			// LOG ERROR
			UE_LOG(LogTemp, Warning, TEXT("UStaticMeshComponent TargetLight is null"));
		}
		Fans[i].ElseLight_ = Fans[i].ElseLight ? Cast<UStaticMeshComponent>(Fans[i].ElseLight->GetComponentByClass(UStaticMeshComponent::StaticClass())) : nullptr;
		for (auto &j : Fans[i].ScoreLights)
		{
			if (j)
			{
				Fans[i].ScoreLights_.Add(Cast<UStaticMeshComponent>(j->GetComponentByClass(UStaticMeshComponent::StaticClass())));

			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Actor in ScoreLights is null"));
			}
		}
	}
}
void UWindContorl::TestLogName(AActor *use_, FString use_string)
{
	if (use_ && !use_->Tags.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s attached to %s"), *use_->Tags[0].ToString(), *use_string);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid actor or label for %s attachment"), *use_string);
	}
}

void UWindContorl::ChangeMode()
{
	// 获取当前时间 ms
	float ElapsedTime = GetWorld()->GetTimeSeconds();
	int TimeUse = static_cast<int>(ElapsedTime * 1000);

	if (ElapsedTime - WindState.LastHitTime > 2.5 || WindState.HitNum == 0)
	{
		WindState.LastHitTime = ElapsedTime; // 超过2S还未击打就更新下一片
		for (int i = 0; i <= 4; i++)
		{
			WindState.state[i] = FAN_MODE::NOACTIVATE;
			Fans[i].Score = 0;
			Fans[i].AlreadyHit = 0;
			if (Fans[i].TargetLight)
			{
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
			if (WindState.state[j] == FAN_MODE::ACTIVED)
				UseNum = UseNum + 1;
			else
			{
				WindState.NowHit = j;
				break;
			}
			// while(WindState.)
		}
		// WindState.NowHit
		WindState.state[WindState.NowHit] = FAN_MODE::ACTIVING; // 正在激活状态
		WindState.HitNum = 1;
	}
	// 亮起环数的显示
	if (WindState.state[WindState.NowHit] == FAN_MODE::ACTIVED)
	{
		if (WindState.HitNum != 6)
		{
			WindState.LastHitTime = ElapsedTime;
			if (Fans[WindState.NowHit].fa == nullptr)
				return; // 如果没有风扇 直接退出

			for (auto &j : Fans[WindState.NowHit].FlowLight_)
				if (j)
					if (j && j->GetMaterial(0))
						j->SetMaterial(0, TargetMaterial);

			if (Fans[WindState.NowHit].TargetLight_)
				Fans[WindState.NowHit].TargetLight_->SetMaterial(0, Material[0]);
				// 并且设置它的状态为隐藏
				// 设置Fans[WindState.NowHit].TargetLight的可见性
				if (Fans[WindState.NowHit].TargetLight)
					Fans[WindState.NowHit].TargetLight->SetActorHiddenInGame(true);

			// 这里有可能显示很多  就会出现点亮很多次的BUG
			for (auto& j : Fans[WindState.NowHit].ScoreLights_) {
					j->SetMaterial(0, Material[0]);
				}

			if (Fans[WindState.NowHit].ScoreLights_[Fans[WindState.NowHit].Score - 1] && !Fans[WindState.NowHit].AlreadyHit)
			{
				Fans[WindState.NowHit].ScoreLights_[Fans[WindState.NowHit].Score - 1]->SetMaterial(0, TargetMaterial);
				Fans[WindState.NowHit].AlreadyHit = 1;
			}

			// 其他地方的显示
			if (Fans[WindState.NowHit].ElseLight_)
			{
				Fans[WindState.NowHit].ElseLight_->SetMaterial(0, TargetMaterial);
			}
			/*		for (auto& j : Fans[WindState.NowHit].ScoreLights_) {
						if (j) {
							j->SetMaterial(0, TargetMaterial);
						}
					}*/
			if (WindState.HitNum == 5)
			{
				WindState.HitNum = 6;
			};
		}

		if (WindState.HitNum != 6)
		{
			int StartNum = FMath::RandRange(0, 5 - WindState.HitNum - 1); // 从第x个数出发后面的第一个可击打的扇叶
			int UseNum = 1;
			for (int i = 1; i <= UseNum; i++)
			{
				int j = (WindState.NowHit + i) % 5;
				if (WindState.state[j] == FAN_MODE::ACTIVED)
					UseNum = UseNum + 1;
				else
				{
					WindState.NowHit = j;
					break;
				}
				// while(WindState.)
			}
			// WindState.NowHit
			WindState.state[WindState.NowHit] = FAN_MODE::ACTIVING; // 正在激活状态
			WindState.HitNum = WindState.HitNum + 1;
		}
	}
	WindRender();
}

void UWindContorl::Settings()
{
	// 按键检测切换 模式显示
	float TimeNow = GetWorld()->GetTimeSeconds();
	if (PlayerController->IsInputKeyDown(EKeys::V) && TimeNow - KeyState.LastSpinModeTime > 0.5)
	{
		KeyState.LastSpinModeTime = TimeNow; // 修复问题1: 添加表达式
		if (WindState.SpinMode == SPIN_MODE::SMALL)
		{
			GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("Change To Big Mode"));
			WindState.LastSpinMode = SPIN_MODE::SMALL;
			WindState.SpinMode = SPIN_MODE::BIG;
			WindState.a = FMath::RandRange(0.780f, 1.045f);		// a 的取值范围为 0.780~1.045
			WindState.omega = FMath::RandRange(1.884f, 2.000f); // ω的取值范围为 1.884~2.000
			// ：spd = a ∗ sin(𝜔𝜔 ∗ 𝑡𝑡) + 𝑏𝑏，其中 spd 的单位
			// 为 rad / s，t 的单位为 s，a 的取值范围为 0.780~1.045，ω的取值范围为 1.884~2.000，b 始终满足 b = 2.090 -
			// a
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("Change To Small Mode"));
			WindState.LastSpinMode = SPIN_MODE::BIG;
			WindState.SpinMode = SPIN_MODE::SMALL;
		}
	} // 修复问题2: 添加分号
	else if (PlayerController->IsInputKeyDown(EKeys::C) && TimeNow - KeyState.LastColorModeTime > 0.5)
	{
		KeyState.LastColorModeTime = GetWorld()->GetTimeSeconds();
		if (WindState.Color == COLOR_GAME::RED)
		{
			GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("Change To Blue Mode"));
			WindState.Color = COLOR_GAME::BLUE;
			TargetMaterial = Material[2];
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("Change To Red Mode"));
			WindState.Color = COLOR_GAME::RED;
			TargetMaterial = Material[1];
		}
		ColorChange = true;
	}
	int Score_ = 0;
	if (TimeNow - KeyState.LastActivateModeTime > 0.3)
	{
		if (PlayerController->IsInputKeyDown(EKeys::Zero))
			Score_ = 10;
		else if (PlayerController->IsInputKeyDown(EKeys::Nine))
			Score_ = 9;
		else if (PlayerController->IsInputKeyDown(EKeys::Eight))
			Score_ = 8;
		else if (PlayerController->IsInputKeyDown(EKeys::Seven))
			Score_ = 7;
		else if (PlayerController->IsInputKeyDown(EKeys::Six))
			Score_ = 6;
		else if (PlayerController->IsInputKeyDown(EKeys::Five))
			Score_ = 5;
		else if (PlayerController->IsInputKeyDown(EKeys::Four))
			Score_ = 4;
		else if (PlayerController->IsInputKeyDown(EKeys::Three))
			Score_ = 3;
		else if (PlayerController->IsInputKeyDown(EKeys::Two))
			Score_ = 2;
		else if (PlayerController->IsInputKeyDown(EKeys::One))
			Score_ = 1;
	}
	if (RandomHit)
	{
		// 假设已经在 KeyState 中添加了 LastRandomHitTime 记录上次随机赋值的时间
		if (TimeNow - KeyState.LastRandomHitTime > 1.8  && WindState.LastSetWinId!= WindState.NowHit)
		{
			// 随机生成 1 到 10 之间的值
			Score_ = FMath::RandRange(1, 10);
			//Score_ = 3;

			// 更新随机赋值的时间
			KeyState.LastRandomHitTime = TimeNow;
			WindState.LastSetWinId = WindState.NowHit;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Now Hit : (%d)  RandomHit Score: (%d) WindState.LastSetWinId  (%d,%d)"),WindState.NowHit, Score_, WindState.LastSetWinId, WindState.NowHit));
			// 输出调试信息（可选）
		}
	}

	if (Score_ && WindState.HitNum<5  )
	{
		GEngine->AddOnScreenDebugMessage(3, 8.f, FColor::White, FString::Printf(TEXT("Activate fan[%d]=%d NowHitNum=%d"), WindState.NowHit, Score_, WindState.HitNum));
		// WindState.HitNum += 1;
		WindState.AllScore += Score_;
		Fans[WindState.NowHit].Score = Score_;
		Fans[WindState.NowHit].MODE = FAN_MODE::ACTIVED; // 已经被激活
		WindState.state[WindState.NowHit] = FAN_MODE::ACTIVED;
		KeyState.LastActivateModeTime = TimeNow;
		WindState.LastHitTime = TimeNow;
		WindState.LastState[WindState.NowHit] = FAN_MODE::ACTIVED;
	}

	//else if ((PlayerController->IsInputKeyDown(EKeys::S) && PlayerController->IsInputKeyDown(EKeys::LeftControl)&& TimeNow - KeyState.LastSaveTime > 0.5)||(AutoSave && TimeNow - KeyState.LastSaveTime > AutoSaverTime))
	//{
	//	KeyState.LastSaveTime = TimeNow;
	//	if (SceneCapture)
	//	{
	//		// GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("SceneCapture is OK"));

	//		if (SceneCapture->TextureTarget)
	//		{
	//			// 我的点总是比我的图像要超前
	//			// FPlatformProcess::Sleep(0.1);
	//			// 击打的中心点 
	//			FVector FanCenterUse = (Fans[WindState.NowHit].ScoreLights[9]->GetActorLocation()); // 我这个时候获取的点
	//			//WorldPointsConvertToSceen
	//			ProjectWorldTo2DSceneCapture(SceneCapture, FanCenterUse, FanCenterUse2D);

	//			//FanCenterUse = FanCenterUse;
	//			WorldPointsConvertToSceen(FanCenterUse, SceneCapture);
	//			//ProjectWorldTo2DSceneCapture(FanCenterUse, SceneCapture, SceneCapture);

	//			// GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("SceneCapture->TextureTarget is OK"));
	//			// FTextureRenderTargetResource *TextureRenderTargetResource = SceneCapture->TextureTarget->GameThread_GetRenderTargetResource();
	//			
	//			int32 Width = SceneCapture->TextureTarget->SizeX;  // 获取高度
	//			int32 Height = SceneCapture->TextureTarget->SizeY; // 获取宽度
	//			// SaveRenderTargetToDisk(SceneCapture->TextureTarget, TEXT("D:/RM/WindExport/1.jpg"), Width, Height, 1);
	//			TArray<FColor> OutData;							  // 声明一个Fcolor数组
	//			SceneCapture->CaptureScene(); // Capture the scene
	//			TextureRenderTargetResource->ReadPixels(OutData); // 读取像素点
	//			FString FormattedTime = FDateTime::Now().ToString();
	//			ColorToImage(TEXT("D:/RM/WindExport/image/") + FormattedTime + TEXT(".png"), OutData, Width, Height); // 写入到本地存成图片
	//			WriteData(TEXT("D:/RM/WindExport/"), FormattedTime);
	//			SavedImageNum+=1;
	//			// Draw circles on OutData at the positions of KeyPoints
	//			// for (const auto& KeyPoint : KeyPoints)
	//			// {
	//			// 	int32 CenterX = FMath::Clamp(static_cast<int32>(KeyPoint.p.X), 0, Width - 1);
	//			// 	int32 CenterY = FMath::Clamp(static_cast<int32>(KeyPoint.p.Y), 0, Height - 1);
	//			// 	int32 Radius = 20;
	//			// 	for (int32 Y = CenterY - Radius; Y <= CenterY + Radius; Y++)
	//			// 	{
	//			// 		for (int32 X = CenterX - Radius; X <= CenterX + Radius; X++)
	//			// 		{
	//			// 			if (FMath::Sqrt(static_cast<float>(FMath::Square(X - CenterX) + FMath::Square(Y - CenterY))) <= Radius)
	//			// 			{
	//			// 				int32 Index = Y * Width + X;
	//			// 				if (OutData.IsValidIndex(Index))
	//			// 				{
	//			// 					OutData[Index] = FColor(0, 255, 0, 255); // Green color
	//			// 				}
	//			// 			}
	//			// 		}
	//			// 	}
	//			// }
	//			// // Save the modified image
	//			// ColorToImage(GerImgName(TEXT("D:/RM/WindExport/"), FormattedTime + TEXT("_marked")), OutData, Width, Height);
	//			// FPlatformProcess::Sleep(0.1);


	//		}
	//		else
	//		{
	//			GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("SceneCapture->TextureTarget is null"));
	//		}
	//	}
	//	else
	//	{
	//		GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("SceneCapture is null"));
	//	}
	//}
	else if (PlayerController->IsInputKeyDown(EKeys::L) && PlayerController->IsInputKeyDown(EKeys::LeftControl) && TimeNow - KeyState.LastCanUseTime > 0.5){
		KeyState.LastCanUseTime=TimeNow;
		CanMove=!CanMove;
	}
	else if (PlayerController->IsInputKeyDown(EKeys::A) && PlayerController->IsInputKeyDown(EKeys::LeftControl) && TimeNow - KeyState.SaveModeTime > 0.5){
		KeyState.SaveModeTime=TimeNow;
		AutoSave=!AutoSave;
	}

	// 多相机取图
	if ((PlayerController->IsInputKeyDown(EKeys::T) && PlayerController->IsInputKeyDown(EKeys::LeftControl) && TimeNow - KeyState.LastSaveTime > 2.5) || (AutoSave && TimeNow - KeyState.LastSaveTime > AutoSaverTime))
	{
		KeyState.LastSaveTime = TimeNow;
		GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::White, FString::Printf(TEXT("CamActors length: %d"), CamActors.Num()));
		int i = 0;
		for (auto SceneCaptureActor: CamActors){
			//SceneCapture= SceneCaptureActor
			SceneCapture = Cast<USceneCaptureComponent2D>(SceneCaptureActor->GetComponentByClass(USceneCaptureComponent2D::StaticClass()));
			if (SceneCapture)
			{
				//GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, FString::Printf(TEXT("%d"),i));
				

				// GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("SceneCapture is OK"));
				if (SceneCapture->TextureTarget)
				{
					i = i + 1;

					// 我的点总是比我的图像要超前
					// FPlatformProcess::Sleep(0.1);
					FVector FanCenterUse = (Fans[WindState.NowHit].ScoreLights[9]->GetActorLocation()); // 我这个时候获取的点
					WorldPointsConvertToSceen(FanCenterUse, SceneCapture);
					// GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("SceneCapture->TextureTarget is OK"));
					// FTextureRenderTargetResource *TextureRenderTargetResource = SceneCapture->TextureTarget->GameThread_GetRenderTargetResource();

					int32 Width = SceneCapture->TextureTarget->SizeX;  // 获取高度
					int32 Height = SceneCapture->TextureTarget->SizeY; // 获取宽度
					// SaveRenderTargetToDisk(SceneCapture->TextureTarget, TEXT("D:/RM/WindExport/1.jpg"), Width, Height, 1);
					TArray<FColor> OutData;							  // 声明一个Fcolor数组
					SceneCapture->CaptureScene(); // Capture the scene
					TextureRenderTargetResource->ReadPixels(OutData); // 读取像素点
					FString FormattedTime = FDateTime::Now().ToString();
					FormattedTime += FString::Printf(TEXT("_%d"), i);
					//UE_LOG(LogTemp, Display, TEXT("多相机取图%s"),FormattedTime);
					//UE_LOG(LogTemp, Display, TEXT("多相机取图: %s"), *FormattedTime);
					//UE_LOG(LogTemp, Display, TEXT("相机位置: %d"), SceneCaptureActor->GetActorLocation()); //fix
					UE_LOG(LogTemp, Display, TEXT("相机位置: %s"), *SceneCaptureActor->GetActorLocation().ToString());


					ColorToImage(TEXT("D:/RM/WindExport/image/") + FormattedTime + TEXT(".png"), OutData, Width, Height); // 写入到本地存成图片
					WriteData(TEXT("D:/RM/WindExport/"), FormattedTime);
					SavedImageNum += 1;
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("SceneCapture->TextureTarget is null"));
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::White, TEXT("SceneCapture is null"));
			}
		}
	}
	if (CanMove && PlayerController->IsInputKeyDown(EKeys::LeftControl)==0 && PlayerController->IsInputKeyDown(EKeys::LeftShift) == 0)
	{
		// 接受预置输入的 X Y Z Roationg
		if ((PlayerController->IsInputKeyDown(EKeys::W) || PlayerController->IsInputKeyDown(EKeys::A) || PlayerController->IsInputKeyDown(EKeys::S) || PlayerController->IsInputKeyDown(EKeys::D) || PlayerController->IsInputKeyDown(EKeys::Q) || PlayerController->IsInputKeyDown(EKeys::E)) && TimeNow - KeyState.LastChangeCamLocationTime>0.05)
		{
			 KeyState.LastChangeCamLocationTime = TimeNow;
			// W A S D 控制相机组件移动
			// Cam
			FVector CamLocation = CamActor->GetActorLocation();
			float BaseMove = 5;
			if (PlayerController->IsInputKeyDown(EKeys::W))
			{
				CamLocation.Z -= BaseMove;
				//GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Yellow, FString::Printf(TEXT("W Pressed CamLocation.Y is %f"), CamLocation.Y));
			}
			else if (PlayerController->IsInputKeyDown(EKeys::S))
			{
				CamLocation.Z += BaseMove;
			}
			if (PlayerController->IsInputKeyDown(EKeys::A))
			{
				CamLocation.Y -= BaseMove;
			}
			else if (PlayerController->IsInputKeyDown(EKeys::D))
			{
				CamLocation.Y += BaseMove;
			}

			if (PlayerController->IsInputKeyDown(EKeys::Q))
			{
				CamLocation.X -= BaseMove;
			}
			else if (PlayerController->IsInputKeyDown(EKeys::E))
			{
				CamLocation.X += BaseMove;
			}
			// 更新位置
			CamActor->SetActorLocation(CamLocation);


		}
	}
	
   // rotation control
	//if (CanMove && PlayerController->IsInputKeyDown(EKeys::Left) == 0)
	if (CanMove && PlayerController->IsInputKeyDown(EKeys::LeftControl)==0 && PlayerController->IsInputKeyDown(EKeys::LeftShift))
	{
		// 接受旋转输入
		if ((PlayerController->IsInputKeyDown(EKeys::W) || PlayerController->IsInputKeyDown(EKeys::A) || PlayerController->IsInputKeyDown(EKeys::S) || PlayerController->IsInputKeyDown(EKeys::D) || PlayerController->IsInputKeyDown(EKeys::Q) || PlayerController->IsInputKeyDown(EKeys::E)) && TimeNow - KeyState.LastChangeCamLocationTime > 0.05)
		{
			KeyState.LastChangeCamLocationTime = TimeNow;

			// 旋转角度调整
			FRotator CamRotation = CamActor->GetActorRotation();
			float BaseRotation = 0.3f;

			if (PlayerController->IsInputKeyDown(EKeys::W))
			{
				CamRotation.Pitch -= BaseRotation;
			}
			else if (PlayerController->IsInputKeyDown(EKeys::S))
			{
				CamRotation.Pitch += BaseRotation;
			}

			if (PlayerController->IsInputKeyDown(EKeys::A))
			{
				CamRotation.Yaw+= BaseRotation;
			}
			else if (PlayerController->IsInputKeyDown(EKeys::D))
			{
				CamRotation.Yaw -= BaseRotation;
			}

			if (PlayerController->IsInputKeyDown(EKeys::Q))
			{
				CamRotation.Roll -= BaseRotation;
			}
			else if (PlayerController->IsInputKeyDown(EKeys::E))
			{
				CamRotation.Roll += BaseRotation;
			}

			// 更新旋转角度
			CamActor->SetActorRotation(CamRotation);
		}
	}
	if (PlayerController->IsInputKeyDown(EKeys::LeftControl) && PlayerController->IsInputKeyDown(EKeys::R) && TimeNow - KeyState.LastChangeCamLocationTime > 0.5)
	{
		KeyState.LastChangeCamLocationTime = TimeNow;

		// 设置摄像机初始位置和旋转
		//FVector InitLocation(750.0f, 0.0f, 236.0f);
		//FRotator InitRotation(0.0f, 0.0f, 180.0f);

		CamActor->SetActorLocation(InitLocation);
		CamActor->SetActorRotation(InitRotation);

		// 输出调试信息
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Camera reset to initial position and rotation."));
	}
// 在当前位置创建一个取图的相机
	if (PlayerController->IsInputKeyDown(EKeys::P) && PlayerController->IsInputKeyDown(EKeys::LeftControl) && TimeNow - KeyState.LastChangeCamLocationTime > 0.5) {
		KeyState.LastChangeCamLocationTime = TimeNow;
		KeyState.bWasInputActive = !KeyState.bWasInputActive;
		// CamActor 
		//if (KeyState.bWasInputActive)
		if(1)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Template = CamActor;

			AActor* NewCamActor = GetWorld()->SpawnActor<AActor>(CamActor->GetClass(), CamActor->GetActorLocation(), CamActor->GetActorRotation(), SpawnParams);
			NewCamActor->SetActorLocation(CamActor->GetActorLocation());
			NewCamActor->SetActorRotation(CamActor->GetActorRotation());
			UE_LOG(LogTemp, Display, TEXT("相机-位置: %s"), *CamActor->GetActorLocation().ToString());
			// 从原 Actor 上获取 SceneCaptureComponent2D

			USceneCaptureComponent2D* OldSceneCapture = Cast<USceneCaptureComponent2D>(
				CamActor->GetComponentByClass(USceneCaptureComponent2D::StaticClass()));

			if (OldSceneCapture)
			{
				// 使用 NewCamActor 作为 Outer 复制该组件
				USceneCaptureComponent2D* NewSceneCapture = DuplicateObject<USceneCaptureComponent2D>(
					OldSceneCapture, NewCamActor);

				if (NewSceneCapture)
				{
					// 将新复制的组件附加到 NewCamActor 的根组件上
					NewSceneCapture->AttachToComponent(
						NewCamActor->GetRootComponent(),
						FAttachmentTransformRules::KeepRelativeTransform);
					// 注册组件，确保其在运行时有效
					NewSceneCapture->RegisterComponent();

					// 如果需要保存引用到全局变量 SceneCapture
					SceneCapture = NewSceneCapture;
				}
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("复制成功"));
			}
			CamActors.Add(CamActor);
			CamActor = NewCamActor;
			SceneCapture = Cast<USceneCaptureComponent2D>(NewCamActor->GetComponentByClass(USceneCaptureComponent2D::StaticClass()));

			//if (!SceneCapture)/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("SceneCapture是空的"));*/
			//UE_LOG(LogTemp, Display, TEXT("复制出新的相机 Actor"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("复制出新的相机Actor"));
		}

	}
	// 设置能量机关的速度
	if (WindState.LastSpinMode == WindState.SpinMode && WindState.SpinMode == SPIN_MODE::SMALL)
		return;
	FRotator Rotator = WindSpinContorl->RotationRate;
	if (WindState.SpinMode == SPIN_MODE::SMALL)
	{
		// 将yaw设置为60
		WindSpinContorl->RotationRate.Yaw = 60;
		// WindSpinContorl->RotationRate.Yaw = 1;
	}
	else
	{
		// 将yaw设置为120
		float angle = WindState.a * FMath::Sin(WindState.omega * GetWorld()->GetTimeSeconds()) + (2.090f - WindState.a);
		angle = FMath::RadiansToDegrees(angle);
		WindSpinContorl->RotationRate.Yaw = angle; // spd = a ∗ sin(𝜔𝜔 ∗ 𝑡𝑡) + 𝑏𝑏，其中 spd 的单位为 rad / s，
												//    WindSpinContorl->RotationRate.Yaw = 1;
	}
}

// 游戏用到的所有的初始化
void UWindContorl::GameInit()
{
	LoadMaterial();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);
	// 输出所有的ActorName
	for (AActor *Actor : FoundActors)
	{
		/*if (Actor->GetActorLabel().Equals(TEXT("FlowLight"))) {
			FlowLightActor = Actor;
		}*/
		// UE_LOG(LogTemp, Display, TEXT("%s"), *Actor->GetActorLabel());
		if (Actor->Tags.IsEmpty())
			continue;
		// UE_LOG(LogTemp, Display, TEXT("%s"), *Actor->Tags[0].ToString());

		if (Actor->Tags[0].ToString() == (TEXT("WindSpin-1")))
		{ // 注意是就等于
			WindSpinActor = Actor;
			// 设置WindSpinActor的Yaw为0  便于寻找
			(Actor->GetRootComponent())->SetMobility(EComponentMobility::Movable);
			FRotator RotatorUse = WindSpinActor->GetActorRotation();
			RotatorUse.Pitch = 0;
			UE_LOG(LogTemp, Display, TEXT("SpinActor Find Fine"));
			WindSpinActor->SetActorRotation(RotatorUse);
			WindSpinContorl = Cast<URotatingMovementComponent>(Actor->GetComponentByClass(URotatingMovementComponent::StaticClass()));
		}
		if (Actor->Tags[0].ToString().Contains(TEXT("GameCamMain")))
		{ // 注意是就等于
			UE_LOG(LogTemp, Display, TEXT("GameCamMainActor Find Fine"));
			CamActor = Actor;
			Cam = Cast<UCameraComponent>(Actor->GetComponentByClass(UCameraComponent::StaticClass()));
			// 设置初始位姿
			InitLocation = CamActor->GetActorLocation();
			InitRotation = CamActor->GetActorRotation();
			
			// Cam->SetMobility(EComponentMobility::Movable);
			SceneCapture = Cast<USceneCaptureComponent2D>(Actor->GetComponentByClass(USceneCaptureComponent2D::StaticClass()));
			if (SceneCapture)
			{
				// 输出Cam的曝光
				//	Cam->PostProcessSettings.expo
				UE_LOG(LogTemp, Display, TEXT("SceneCapture Fine"));
				// 保存图像
				// SceneCapture->CaptureScene();
			}
		}

		if (Actor->Tags[0].ToString().Contains(TEXT("SpotLight")))
		{ // 注意是就等于
			// UE_LOG(LogTemp, Display, TEXT("SpotLight %s Find"), *Actor->Tags[0].ToString());
			EnvironmentLights.Add(Cast<ULightComponent>(Actor->GetComponentByClass(ULightComponent::StaticClass())));
		}
		if (Actor->Tags[0].ToString().Contains(TEXT("MainPart1")))
		{
			WindMainPart1 = Cast<UStaticMeshComponent>(Actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			if (WindMainPart1)
			{
				WindMainPart1->SetMaterial(0, Material[0]);
				// UE_LOG(LogTemp, Display, TEXT("MainPart1 Find Fine"));
			}
		}
		if (Actor->Tags[0].ToString().Contains(TEXT("MainPart2")))
		{
			WindMainPart2 = Cast<UStaticMeshComponent>(Actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			if (WindMainPart2)
			{
				WindMainPart2->SetMaterial(0, Material[0]);
				// UE_LOG(LogTemp, Display, TEXT("MainPart2 Find Fine"));
			}
		}
		if (Actor->Tags[0].ToString().Contains(TEXT("WindSpin-1_R-1_RLight-1")))
		{
			CenterActorUse = Actor;
			CenterLight_ = Cast<UStaticMeshComponent>(Actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			if (CenterLight_)
			{
				CenterLight_->SetMaterial(0, TargetMaterial);
				// UE_LOG(LogTemp, Display, TEXT("RLight Find Fine"));
			}
		}
	}
	GetFans();

	for (int i = 0; i <= 4; i++)
	{
		WindState.state[i] = FAN_MODE::NOACTIVATE;
		WindState.LastState[i] = FAN_MODE::ACTIVED;
		for (auto &j : Fans[i].FlowLight_)
		{
			if (j)
			{
				if (j && j->GetMaterial(0))
				{
					j->SetMaterial(0, Material[0]);
				}
			}
		}
		if (Fans[i].TargetLight_)
		{
			Fans[i].TargetLight_->SetMaterial(0, Material[0]);
		}
		if (Fans[i].ElseLight_)
		{
			Fans[i].ElseLight_->SetMaterial(0, Material[0]);
		}
		for (auto &j : Fans[i].ScoreLights_)
		{
			if (j)
			{
				j->SetMaterial(0, Material[0]);
			}
		}
	}
	// 获得我们的玩家控制器
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, TEXT("Play Get Ok"));
		PlayerController->EnableInput(PlayerController);
	}
	TextureRenderTargetResource = SceneCapture->TextureTarget->GameThread_GetRenderTargetResource();

}
void UWindContorl::LoadMaterial()
{
	Material[0] = LoadObject<UMaterialInstanceConstant>(
		nullptr,
		TEXT("/Game/WIND/Materials/color_1098367027_.color_1098367027_"),
		nullptr,
		LOAD_None,
		nullptr);
	/// Script / Engine.MaterialInstanceConstant'/Game/Wind/WIND/Materials/color_1098367027_.color_1098367027_'
	Material[1] = LoadObject<UMaterialInstanceConstant>(
		nullptr,
		TEXT("/Game/WIND/Materials/red_LED_1814320893_.red_LED_1814320893_"),
		nullptr,
		LOAD_None,
		nullptr);
	Material[2] = LoadObject<UMaterialInstanceConstant>(
		nullptr,
		TEXT("/Game/WIND/Materials/blue_led_1230032108_.blue_led_1230032108_"),
		nullptr,
		LOAD_None,
		nullptr);
	if (Material[0])
	{
		TargetMaterial = Material[1];
	}
	// 初始化3D点数据库
	WorldPointsInit();
	/// Script / Engine.MaterialInstanceConstant'/Game/Wind/WIND/Materials/white_low_gloss_plastic_1767808287_.white_low_gloss_plastic_1767808287_'
}
/// Script / Engine.MaterialInstanceConstant''
void UWindContorl::WindRender()
{
	// 根据状态渲染我们的能量机关
	// if(UMaterialInstanceConstant[0])
	for (int i = 0; i <= 2; i++)
	{
		if (Material[i] == nullptr)
			return; // 如果没有材料 直接退出
	}
	for (int i = 0; i <= 4; i++)
	{
		if (Fans[i].fa == nullptr)
			return; // 如果没有风扇 直接退出
		if (WindState.state[i] == WindState.LastState[i] && WindState.NowHit != i && !ColorChange)
			continue; // 如果没有变化并且不需要流水就不渲染这一帧率
		if (WindState.state[i] == FAN_MODE::NOACTIVATE)
		{
			// 只有一种情况那就是原来是亮起状态
			WindState.LastState[i] = FAN_MODE::NOACTIVATE;
			for (auto &j : Fans[i].FlowLight_)
				if (j)
					if (j && j->GetMaterial(0))
						j->SetMaterial(0, Material[0]);

			if (Fans[i].TargetLight_)
				Fans[i].TargetLight_->SetMaterial(0, Material[0]);
			if (Fans[i].ElseLight_)
				Fans[i].ElseLight_->SetMaterial(0, Material[0]);
			for (auto &j : Fans[i].ScoreLights_)
				if (j) j->SetMaterial(0, Material[0]);
		}
		else if (WindState.state[i] == FAN_MODE::ACTIVING)
		{

			float ElapsedTime = GetWorld()->GetTimeSeconds();
			int FlowUseState = static_cast<int>(ElapsedTime * 10) % 3;
			// 输出FlowUseState
			// UE_LOG(LogTemp, Display, TEXT("FlowUseState is %d"), FlowUseState);
			if (FlowUseState != WindState.LastFlowUseState || ColorChange)
			{
				WindState.LastFlowUseState = FlowUseState;
				for (auto &j : Fans[i].FlowLight_)
					if (j)
						if (j && j->GetMaterial(0))
							j->SetMaterial(0, Material[0]);
				Fans[i].FlowLight_[FlowUseState + 1]->SetMaterial(0, TargetMaterial);
			}
			// 静态的亮起只更新一次
			if (WindState.LastState[i] != FAN_MODE::ACTIVING || ColorChange)
			{
				for (auto &j : Fans[i].ScoreLights_)
				{
					if (j)
					{
						j->SetMaterial(0, Material[0]);
					}
				}
				if (Fans[i].ElseLight_)
				{
					Fans[i].ElseLight_->SetMaterial(0, Material[0]);
				}
				// 记得更新颜色 Material[1]全局替换即可
				if (Fans[i].TargetLight_)
				{
					Fans[i].TargetLight_->SetMaterial(0, TargetMaterial);
				}
				WindState.LastState[i] = FAN_MODE::ACTIVING;
			}
		}
	}
	if (ColorChange)
	{
		ColorChange = false;
		CenterLight_->SetMaterial(0, TargetMaterial);
	}
	// for(int)
}

void UWindContorl::ColorToImage(const FString &InImagePath, TArray<FColor> InColor, int32 InWidth, int32 InHight)
{

	IImageWrapperModule &ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");
	FString Ex = FPaths::GetExtension(InImagePath);
	if (Ex.Equals(TEXT("jpg"), ESearchCase::IgnoreCase) || Ex.Equals(TEXT("jpeg"), ESearchCase::IgnoreCase))
	{
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
		// 往本地写，获取颜色数据，获取尺寸，获取长度，高度，格式rgb,8位
		if (ImageWrapper->SetRaw(InColor.GetData(), InColor.GetAllocatedSize(), InWidth, InHight, ERGBFormat::BGRA, 8))
		{
			FFileHelper::SaveArrayToFile(ImageWrapper->GetCompressed(100), *InImagePath);
		}
	}
	else
	{
		TArray<uint8> OutPNG;

		for (FColor &color : InColor)
		{
			color.A = 255; // 不写的画截屏的图片都是透明通道，就是透明的
		}
		FImageUtils::CompressImageArray(InWidth, InHight, InColor, OutPNG);
		FFileHelper::SaveArrayToFile(OutPNG, *InImagePath);
	}
}

FString UWindContorl::GerImgName(FString Path, FString Name)
{
	return Path + TEXT("image/") + Name + TEXT(".png");
}

bool UWindContorl::WriteData(FString Path, FString Name)
{
	FString FormattedTime = Path + TEXT("data/") + Name + TEXT(".yml");
	// COCO CocoUse;
	TArray<FString> TextLines;
	TextLines.Add(FString::Printf(TEXT("ID: %s"), *Name));
	TextLines.Add(FString::Printf(TEXT("NowHitId: %d"), WindState.NowHit));

	TextLines.Add(TEXT("Areas: "));
	for(auto &Area:Areas)
	{                     //1234
		TextLines.Add(FString::Printf(TEXT("  - Name: %s"),*(Area.AreaName)));
		TextLines.Add(FString::Printf(TEXT("    Area: [%f,%f,%f,%f]"),(Area.LeftTop.X),(Area.LeftTop.Y),(Area.RightBottom.X),(Area.RightBottom.Y)));
	}
	TextLines.Add(TEXT("KeyPoints: "));
	//int i = 0;
	for(auto &KeyPoint:KeyPoints)
	{
		//i = i + 1;
		//if (i > 26) break;
		TextLines.Add(FString::Printf(TEXT("  - Name: %s"),*(KeyPoint.PointName)));
		TextLines.Add(FString::Printf(TEXT("    Point: [%f,%f]"),(KeyPoint.p.X),(KeyPoint.p.Y)));
	}

	TextLines.Add(TEXT("Camera Postion: "));
	TextLines.Add(TEXT("Camera Rotation: "));

	//TextLines.Add(TEXT("HitCenterPoints: "));
	//TextLines.Add(FString::Printf(TEXT("    Point: [%f,%f]"), (FanCenterUse2D.X), (FanCenterUse2D.Y)));

	//TextLines.Add(FString::Printf(TEXT("  - Name: %f"), FanCenterUse);

	FFileHelper::SaveStringArrayToFile(TextLines, *FormattedTime, FFileHelper::EEncodingOptions::ForceUTF8);
	return 0;
}

TArray<FVector2D> UWindContorl::WorldToScreen(TArray<FVector> InputWorldPoints)
{
	TArray<FVector2D> ScreenPoints;
	for (FVector &Point : InputWorldPoints)
	{
		FVector2D ScreenPoint;
		PlayerController->ProjectWorldLocationToScreen(Point, ScreenPoint);
		ScreenPoints.Add(ScreenPoint);
	}
	return ScreenPoints;
}

bool UWindContorl::WorldPointsInit()
{
	RPoints.Add(FVector(48.536673, 0.240528, 235.400002)); // 最中心的点
	RPoints.Add(FVector(0.299997, -3.516182, 3.361168));   // R1
	RPoints.Add(FVector(0.299999, -1.017246, 2.442187));   // R2
	RPoints.Add(FVector(0.300002, 2.632324, 3.728447));	   // R3
	RPoints.Add(FVector(0.300002, 2.632325, 1.672881));	   // R4
	RPoints.Add(FVector(0.300004, 3.718210, -3.387157));   // R5
	RPoints.Add(FVector(0.300002, 2.300310, -3.568442));   // R6
	RPoints.Add(FVector(0.300002, -3.516180, -1.523751));  // R7
	RPoints.Add(FVector(0.299999, -1.574683, -3.465232));  // R8
	
	// FanPoints.Add(FVector(0.299999, -1.574683, -3.465232)); //Top1
	FanPoints.Add(FVector2D(1.75,14.897567)); //Top1
	FanPoints.Add(FVector2D(-1.75,14.897567)); //Top1

	FanPoints.Add(FVector2D(1.567193,13.40875)); //Top2
	FanPoints.Add(FVector2D(-1.567193,13.40875)); //Top2

	FanPoints.Add(FVector2D(0.835955,7.453266)); //Top3
	FanPoints.Add(FVector2D(-0.835955,7.453266)); //Top3

	FanPoints.Add(FVector2D(0.000208,5.000000)); //Top4
	FanPoints.Add(FVector2D(-0.000208,5.000000)); //Top4

	//FanPoints.Add(FVector2D(-0.000208, 5.000000)); //Out1
	// 先是扇叶上外面四个点
	OutPoints.Add(FVector(-15.98889, 15.500162, 0.8));  // OUT1
	OutPoints.Add(FVector(15.98889, 15.500162, 0.8));  // OUT2

	OutPoints.Add(FVector(18.418955, -15.931998, 0.8));  // OUT3
	OutPoints.Add(FVector(-18.418955, -15.931998, 0.8));  // OUT4
	// 后是扇叶上里面四个点

	OutPoints.Add(FVector(-17.621916, 3.89999, 0.4));  // OUT4
	OutPoints.Add(FVector(17.621916, 3.89999, 0.4));  // OUT4


	OutPoints.Add(FVector(17.858159, -2.78381, 0.4));  // OUT4
	OutPoints.Add(FVector(-17.858159, -2.78381, 0.4));  // OUT4

  // 再是外面灯条上四个点

	OutPoints.Add(FVector(19.786865, -36.641862, 0.809699));  // OUT4
	OutPoints.Add(FVector(-19.786865, -36.641862, 0.809699));  // OUT4

	OutPoints.Add(FVector(6.985479, -54.303495, 0.809699));  // OUT4
	OutPoints.Add(FVector(-6.985479, -54.303495, 0.809699));  // OUT4

	for(int i=1;i<=3;i++)
	{
		for(int j=0+8*(i-1);j<=8*i-1;j++) // i=1  0-7  i=2 
		{
			FVector2D Use=FanPoints[j];
			FanPoints.Add(FVector2D(Use.Y,-Use.X));
		}
	}

	// Z=Z Y=-Y
	// WindPoints
	return true;
}

FVector2d UWindContorl::ConverToImgSize(FVector2d UseVec)
{
	FVector2D ViewSize;
	FVector2D RealVec;
	GEngine->GameViewport->GetViewportSize(ViewSize);

	// UE_LOG(LogTemp, Log, TEXT("ConverToImgSize UseVec: X=%f, Y=%f"), UseVec.X, UseVec.Y);
	// UE_LOG(LogTemp, Log, TEXT("ConverToImgSize ViewSize: X=%f, Y=%f"), ViewSize.X, ViewSize.Y);
// X=459.716919, Y=417.146637
// LogTemp: ConverToImgSize ViewSize: X=902.000000, Y=848.000000
	float ratio = 1440 / 1080;
	float RatioUse = UseVec.X / UseVec.Y;
	if (RatioUse < ratio)
	{
		// for example 1440/1440
		// 对Y做特殊处理
		RealVec.X = UseVec.X / ViewSize.X * 1440;
		RealVec.Y = (UseVec.Y - ((ViewSize.Y - (ViewSize.X / 1440) * 1080) / 2)) * (1440 / ViewSize.X);
	}
	else
	{
		RealVec.Y = UseVec.Y / ViewSize.Y * 1080;
		RealVec.X = (UseVec.X - ((ViewSize.X - (ViewSize.Y / 1080) * 1440) / 2)) * (1080 / ViewSize.Y);
	}
	UE_LOG(LogTemp, Log, TEXT("ConverToImgSize ViewSize: X=%f, Y=%f"), RealVec.X, RealVec.Y);
	return RealVec;

}

bool UWindContorl::WorldPointsConvertToSceen(FVector FanCenter_, USceneCaptureComponent2D* SceneCaptureUse)
{

	// 清除Areas所有元素
	Areas.Empty();
	KeyPoints.Empty();
	FRotator RotatorUse = WindSpinActor->GetActorRotation();
	FRotator RotatorUseSpin(RotatorUse.Pitch, 0.0f, 0.0f);
	FQuat RotationQuat(RotatorUseSpin);
	// 中心点
	TArray<FVector2D> UsePoints;
	FVector2D ViewSize;
	GEngine->GameViewport->GetViewportSize(ViewSize);
	FVector Rcenter = CenterActorUse->GetActorLocation();
	for (int i = 1; i <= 8; i++)
	{
		// FVector RotatedVector3D = RotationQuat.RotateVector(RPoints[i] + RPoints[0]);
		FVector RotatedVector3D = (RPoints[i] + Rcenter);
		FVector2D ScreenPosition;
		//PlayerController->ProjectWorldLocationToScreen(RotatedVector3D, ScreenPosition);
		ProjectWorldTo2DSceneCapture(SceneCaptureUse,RotatedVector3D, ScreenPosition);
		// 获取当前屏幕尺寸
		//UE_LOG(LogTemp, Log, TEXT("Sceen Point Size is %f %f"), ScreenPosition.X, ScreenPosition.Y);
		//ScreenPosition = ConverToImgSize(ScreenPosition);
		UsePoints.Add(ScreenPosition);
		//UE_LOG(LogTemp, Log, TEXT("Projected %d Position: X=%f, Y=%f"), i, ScreenPosition.X, ScreenPosition.Y);
	}
	
	float minX = FLT_MAX, minY = FLT_MAX, maxX = FLT_MIN, maxY = FLT_MIN;
	for (const auto &point : UsePoints)
	{
		minX = FMath::Min(minX, point.X);
		minY = FMath::Min(minY, point.Y);
		maxX = FMath::Max(maxX, point.X);
		maxY = FMath::Max(maxY, point.Y);
	}

	// 计算最小外接矩形的宽度和高度
	float width = maxX - minX;
	float height = maxY - minY;
		int TZ=5;
	float clampedMinX = FMath::Clamp(minX - TZ, 0.f, 1440.f);
	float clampedMinY = FMath::Clamp(minY - TZ, 0.f, 1080.f);
	float clampedMaxX = FMath::Clamp(maxX + TZ, 0.f, 1440.f);
	float clampedMaxY = FMath::Clamp(maxY + TZ, 0.f, 1080.f);

	// UE_LOG(LogTemp, Log, TEXT("Projected %f %f"), width, height);
	Areas.Add({FVector2D(clampedMinX, clampedMinY), FVector2D(clampedMaxX, clampedMaxY), TEXT("Center")});
	// Areas.Add({FVector2D(minX, minY),FVector2D(maxX, maxY),TEXT("Center")});
	
	// 接下来是我们扇叶的部分
	FVector TargetLightCenter=FanCenter_;
	TargetLightCenter.X+=0.5;
	FVector Ix, Iy;
    Iy=TargetLightCenter-RPoints[0];
	Iy.X=0,Ix.X=0;
	// 将向量(x,y)顺时针旋转90度
	Ix.Y = Iy.Z;
	Ix.Z = -Iy.Y;
	// Iy.X=0,Ix.Y=Iy.Z,Ix.Z=-Iy.Y;
	//UE_LOG(LogTemp, Log, TEXT("The Need Hit Fan Center %f %f %f"), TargetLightCenter.X, TargetLightCenter.Y,TargetLightCenter.Z);
	// 归一化Ix 和 Iy
	Ix.Normalize();
	Iy.Normalize();
	FVector2D ScreenPosition;
	//PlayerController->ProjectWorldLocationToScreen(TargetLightCenter, ScreenPosition);
	ProjectWorldTo2DSceneCapture(SceneCaptureUse, TargetLightCenter, ScreenPosition);

	// 获取当前屏幕尺寸
	// UE_LOG(LogTemp, Log, TEXT("Sceen Point Size is %f %f"), ScreenPosition.X, ScreenPosition.Y);
	//ScreenPosition = ConverToImgSize(ScreenPosition);
	KeyPoints.Add({ScreenPosition, FString::Printf(TEXT("TarGetLight"))});
	// 找各个点所对应的投影点
	for(int i=0;i<32;i++){
		FVector2d Use2D=FanPoints[i];
		FVector UseVector=TargetLightCenter+Ix*Use2D.X+Iy*Use2D.Y;
		//FVector2D ScreenPosition;
		//PlayerController->ProjectWorldLocationToScreen(UseVector, ScreenPosition);
		ProjectWorldTo2DSceneCapture(SceneCaptureUse,UseVector, ScreenPosition);

		// 获取当前屏幕尺寸
		//UE_LOG(LogTemp, Log, TEXT("Sceen Point Size is %f %f"), ScreenPosition.X, ScreenPosition.Y);
		//ScreenPosition = ConverToImgSize(ScreenPosition);
		KeyPoints.Add({ScreenPosition, FString::Printf(TEXT("P%d"), i+1)});
	}


	minX = FLT_MAX, minY = FLT_MAX, maxX = FLT_MIN, maxY = FLT_MIN;
	for (const auto &Point : KeyPoints)
	{
		auto point=Point.p;
		minX = FMath::Min(minX, point.X);
		minY = FMath::Min(minY, point.Y);
		maxX = FMath::Max(maxX, point.X);
		maxY = FMath::Max(maxY, point.Y);
	}
	// UE_LOG(LogTemp, Log, TEXT("Projected %f %f"), width, height);
	TZ=5;
	clampedMinX = FMath::Clamp(minX - TZ, 0.f, 1440.f);
	clampedMinY = FMath::Clamp(minY - TZ, 0.f, 1080.f);
	clampedMaxX = FMath::Clamp(maxX + TZ, 0.f, 1440.f);
	clampedMaxY = FMath::Clamp(maxY + TZ, 0.f, 1080.f);

	Areas.Add({FVector2D(clampedMinX, clampedMinY), FVector2D(clampedMaxX, clampedMaxY), TEXT("Target")});
	for (int i = 0; i < 12; i++) {
		FVector2d Use2D = FVector2d(OutPoints[i].X, OutPoints[i].Y);
		FVector UseVector = TargetLightCenter + Ix * Use2D.X + Iy * Use2D.Y;
		//PlayerController->ProjectWorldLocationToScreen(UseVector, ScreenPosition);
		ProjectWorldTo2DSceneCapture(SceneCaptureUse, UseVector, ScreenPosition);

		//ScreenPosition = ConverToImgSize(ScreenPosition);
		KeyPoints.Add({ ScreenPosition, FString::Printf(TEXT("Out%d"), i + 1) });
	}
	return 1;
}

bool UWindContorl::ProjectWorldTo2DSceneCapture(USceneCaptureComponent2D* SceneCaptureComponent2D,const FVector& WorldPosition, FVector2D& ScreenPosition)
{
	if (!SceneCaptureComponent2D || !SceneCaptureComponent2D->TextureTarget) {
		ScreenPosition = FVector2D::ZeroVector;
		//UE_LOG()
		//UE_LOG(LogTemp, Log, TEXT("wrong ProjectWorldTo2DSceneCapture"));
		return false;
	}
	TOptional<FMatrix> CustomProjectionMatrix;
	if (SceneCaptureComponent2D->bUseCustomProjectionMatrix) {
		CustomProjectionMatrix = SceneCaptureComponent2D->CustomProjectionMatrix;
	}
	FMinimalViewInfo ViewInfo;
	SceneCaptureComponent2D->GetCameraView(0.0f, ViewInfo);
	FMatrix OutViewMatrix, ProjectionMatrix, ViewProjectionMatrix;
	UGameplayStatics::CalculateViewProjectionMatricesFromMinimalView(ViewInfo, CustomProjectionMatrix, OutViewMatrix, ProjectionMatrix, ViewProjectionMatrix);
	FIntPoint TargetSize(SceneCaptureComponent2D->TextureTarget->SizeX, SceneCaptureComponent2D->TextureTarget->SizeY);
	FIntRect ViewRect(FIntPoint(0, 0), TargetSize);
	bool bResult = FSceneView::ProjectWorldToScreen(WorldPosition, ViewRect, ViewProjectionMatrix, ScreenPosition);
	//UE_LOG(LogTemp, Log, TEXT("ProjectWorldTo2DSceneCapture %lf %lf"), ScreenPosition.X, ScreenPosition.Y);

	return bResult;
}

//  设置摄像机位置函数