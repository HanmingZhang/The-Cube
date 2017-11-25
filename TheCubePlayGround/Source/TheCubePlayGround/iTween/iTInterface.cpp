// Fill out your copyright notice in the Description page of Project Settings.

#include "iTInterface.h"
#include "TheCubePlayGround.h"
#include "iTweenPCH.h"


UiTInterface::UiTInterface(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void STweenableWidget::OnTweenStart(AiTweenEvent* eventOperator, AActor* actorTweening, USceneComponent* componentTweening, UWidget* widgetTweening, FName tweenName)
{
	;
}

void STweenableWidget::OnTweenTick(AiTweenEvent* eventOperator, AActor* actorTweening, USceneComponent* componentTweening, UWidget* widgetTweening, FName tweenName, float alphaCompletion)
{
	;
}

void STweenableWidget::OnTweenDataTick(AiTweenEvent* eventOperator, FName tweenName, float floatValue, FLinearColor linearColorValue, FRotator rotatorValue, FVector vectorValue, FVector2D vector2DValue, float alphaCompletion)
{
	;
}

void STweenableWidget::OnTweenLoop(AiTweenEvent* eventOperator, AActor* actorTweening, USceneComponent* componentTweening, UWidget* widgetTweening, FName tweenName, int32 numberOfLoopSections, ELoopType::LoopType loopType, bool playingBackward)
{
	;
}

void STweenableWidget::OnTweenComplete(AiTweenEvent* eventOperator, AActor* actorTweening, USceneComponent* componentTweening, UWidget* widgetTweening, FName tweenName, FHitResult sweepHitResultForMoveEvents, bool successfulTransform)
{
	;
}

void STweenableWidget::SetTweenScale(FVector2D Scale)
{
	TweenTransform.Scale = Scale;
	UpdateRenderTransform();
}

void STweenableWidget::SetTweenShear(FVector2D Shear)
{
	TweenTransform.Shear = Shear;
	UpdateRenderTransform();
}

void STweenableWidget::SetTweenAngle(float Angle)
{
	TweenTransform.Angle = Angle;
	UpdateRenderTransform();
}

void STweenableWidget::SetTweenTranslation(FVector2D Translation)
{
	TweenTransform.Translation = Translation;
	UpdateRenderTransform();
}

void STweenableWidget::UpdateRenderTransform()
{
	if (!TweenTransform.IsIdentity())
	{
		//we need to create the transform here, you can not extract the original values so it must be recreated every time there is a change
		FSlateRenderTransform FinalTransform = ::Concatenate(FScale2D(TweenTransform.Scale), FShear2D::FromShearAngles(TweenTransform.Shear), FQuat2D(FMath::DegreesToRadians(TweenTransform.Angle)), FVector2D(TweenTransform.Translation));
		this->SetRenderTransform(FinalTransform);
	}
	else
	{
		this->SetRenderTransform(TOptional<FSlateRenderTransform>());
	}
}
