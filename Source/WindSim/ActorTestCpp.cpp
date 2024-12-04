// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorTestCpp.h"

// Sets default values for this component's properties
UActorTestCpp::UActorTestCpp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	/*UE_LOG(LogTemp, Warning, TEXT("C++ TEST INIT YES"));*/
	// ...
}


// Called when the game starts
void UActorTestCpp::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, TEXT("ActorTestCpp Constructor"));
	// ...
	
}


// Called every frame
void UActorTestCpp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

