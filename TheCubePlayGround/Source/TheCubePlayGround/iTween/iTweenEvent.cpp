// Fill out your copyright notice in the Description page of Project Settings.

#include "iTweenEvent.h"
#include "TheCubePlayGround.h"
#include "iTweenPCH.h"
#include "iTween.h"


AiTweenEvent::AiTweenEvent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AiTweenEvent::BeginPlay()
{
	Super::BeginPlay();
	aux = (AiTAux*)GetOwner();

	UiTween::Print("iTweenEvent object created");
}

void AiTweenEvent::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);

	if (aux)
	{
		UiTween::Print("iTweenEvent object destroyed.");

		aux->currentTweens.Remove(this);
	}

	if (eventType == actorMoveToSplinePoint || eventType == actorRotateToSplinePoint || eventType == compMoveToSplinePoint || eventType == compRotateToSplinePoint)
	{
		if (destroySplineObject)
		{
			splineComponent->GetOwner()->Destroy();
		}
	}
}

void AiTweenEvent::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	deltaSeconds = DeltaSeconds;
	if (ignoreTimeDilation)
	{
		deltaSeconds *= (1 / GetWorldSettings()->TimeDilation);
	}
	ExecuteTween();
}

void AiTweenEvent::UpdateTween()
{
	deltaSeconds = timerInterval;
	ExecuteTween();
}

void AiTweenEvent::SetTimerInterval(float interval /*= 0.f*/)
{
	timerInterval = FMath::Abs(interval);

	if (timerInterval > 0.0001f)
	{
		this->PrimaryActorTick.SetTickFunctionEnable(false);

		UpdateTween();
		
		//FTimerHandle handle = FTimerHandle();

		//GetWorld()->GetTimerManager().SetTimer(handle, &AiTweenEvent::UpdateTween, timerInterval, true);
		//GetWorld()->GetTimerManager().SetTimer(this, &AiTweenEvent::UpdateTween, timerInterval, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

		this->PrimaryActorTick.SetTickFunctionEnable(true);
	}
}

void AiTweenEvent::ParseParameters(FString params)
{
	//Create an array and a map for parsing
	TArray<FString> arr;
	TMap<FString, FString> map;
	//Split into new array entry after each semicolon
	params.ParseIntoArray(arr, TEXT(";"), true);

	//Split each array entry into map at the equal (assignment) sign
	for (int i = 0; i < arr.Num(); i++)
	{
		arr[i].Trim();
		arr[i].TrimTrailing();

		FString left;
		FString right;
		arr[i].Split("=", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);
		left = left.ToLower();
		left.ReplaceInline(TEXT(" "), TEXT(""));
		right.Trim();
		right.TrimTrailing();
		map.Add(left, right);
	}

	if (map.Contains("name"))
	{
		tweenName = FName(*map["name"]);
		UiTween::Print("tweenName: " + tweenName.ToString());
	}

	if (map.Contains("floatfrom"))
	{
		map.Add("ffrom", map["floatfrom"]);
		map.Remove("floatfrom");
	}

	if (map.Contains("ffrom"))
	{
		map["ffrom"].ReplaceInline(TEXT("("), TEXT(" "));
		map["ffrom"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["ffrom"].ReplaceInline(TEXT("["), TEXT(" "));
		map["ffrom"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["ffrom"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["ffrom"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["ffrom"].Trim();
		map["ffrom"].TrimTrailing();

		if (map["ffrom"].Contains("current") || map["ffrom"].Contains("this"))
		{
			if (widgetTweening != nullptr)
			{
				if (eventType == umgRTRotateFromTo)
				{
					floatFrom = widgetTweening->RenderTransform.Angle;
				}
				else
				{
					UiTween::Print("You aren't performing a UMG RT Rotate From/To operation. The 'current' and 'this' keywords only apply to that operation. Defaulting floatFrom to 0.", "error");
					floatFrom = 0.f;
				}
			}
			else
			{
				UiTween::Print("You aren't performing a UMG RT Rotate From/To operation. The 'current' and 'this' keywords only apply to that operation. Defaulting floatFrom to 0.", "error");
				floatFrom = 0.f;
			}
		}
		else
		{
			floatFrom = FCString::Atof(*map["ffrom"]);
		}

		if (map["ffrom"].Contains("*"))
		{
			FString left;
			FString right;
			map["ffrom"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			floatFrom *= FCString::Atof(*right);
		}

		UiTween::Print("floatFrom: " + FString::SanitizeFloat(floatFrom));
	}

	if (map.Contains("floatto"))
	{
		map.Add("fto", map["floatto"]);
		map.Remove("floatto");
	}

	if (map.Contains("fto"))
	{
		map["fto"].ReplaceInline(TEXT("("), TEXT(" "));
		map["fto"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["fto"].ReplaceInline(TEXT("["), TEXT(" "));
		map["fto"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["fto"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["fto"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["fto"].Trim();
		map["fto"].TrimTrailing();

		if (map["fto"].Contains("current") || map["fto"].Contains("this"))
		{
			if (widgetTweening != nullptr)
			{
				if (eventType == umgRTRotateFromTo)
				{
					floatTo = widgetTweening->RenderTransform.Angle;
				}
				else
				{
					UiTween::Print("You aren't performing a UMG RT Rotate From/To operation. The 'current' and 'this' keywords only apply to that operation. Defaulting floatFrom to 0.", "error");
					floatTo = 0.f;
				}
			}
			else
			{
				UiTween::Print("You aren't performing a UMG RT Rotate From/To operation. The 'current' and 'this' keywords only apply to that operation. Defaulting floatFrom to 0.", "error");
				floatTo = 0.f;
			}
		}
		else
		{
			floatTo = FCString::Atof(*map["fto"]);
		}

		if (map["fto"].Contains("*"))
		{
			FString left;
			FString right;
			map["fto"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			floatTo *= FCString::Atof(*right);
		}

		UiTween::Print("floatTo: " + FString::SanitizeFloat(floatTo));
	}

	if (map.Contains("coordinatespace"))
	{
		map.Add("cspace", map["coordinatespace"]);
		map.Remove(map["coordinatespace"]);
	}

	if (map.Contains("cspace"))
	{
		map["cspace"].Trim();
		map["cspace"].TrimTrailing();

		if (map["cspace"].Contains("world"))
		{
			coordinateSpace = CoordinateSpace::world;
		}
		else if (map["cspace"].Contains("parent"))
		{
			coordinateSpace = CoordinateSpace::parent;
		}
		else if (map["cspace"].Contains("self"))
		{
			coordinateSpace = CoordinateSpace::self;
		}

		UiTween::Print("coordinateSpace: " + map["cspace"]);
	}

	if (map.Contains("vectorfrom"))
	{
		map.Add("vfrom", map["vectorfrom"]);
		map.Remove("vectorfrom");
	}

	if (map.Contains("vfrom"))
	{
		map["vfrom"].ReplaceInline(TEXT("("), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT("["), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT("FVector"), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT("X="), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT("Y="), TEXT(","));
		map["vfrom"].ReplaceInline(TEXT("Z="), TEXT(","));

		map["vfrom"].Trim();
		map["vfrom"].TrimTrailing();

		//Implement something that will allow a parameter to be the object's current transform
		if (map["vfrom"].Contains("zero"))
		{
			vectorFrom = FVector::ZeroVector;
		}
		else if (map["vfrom"].Contains("one"))
		{
			vectorFrom = FVector(1, 1, 1);
		}
		else if (map["vfrom"].Contains("current") || map["vfrom"].Contains("this"))
		{
			if (actorTweening != nullptr)
			{
				vectorFrom = actorTweening->GetActorLocation();
			}
			else if (componentTweening != nullptr)
			{
				vectorFrom = componentTweening->GetComponentLocation();
			}
			else
			{
				UiTween::Print("You aren't performing an Actor- or SceneComponent-based operation. The 'current' and 'this' keywords only apply to those operations. Defaulting vectorFrom to (0,0,0).", "error");
				vectorFrom = FVector::ZeroVector;
			}
		}
		else
		{
			TArray<FString> parse;

			map["vfrom"].ParseIntoArray(parse, TEXT(","), true);

			vectorFrom = FVector(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]));
		}

		if (map["vfrom"].Contains("*"))
		{
			FString left;
			FString right;
			map["vfrom"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			vectorFrom *= FCString::Atof(*right);
		}

		UiTween::Print("vectorFrom: " + vectorFrom.ToString());
	}

	if (map.Contains("vectorto"))
	{
		map.Add("vto", map["vectorto"]);
		map.Remove("vectorto");
	}

	if (map.Contains("vto"))
	{
		map["vto"].ReplaceInline(TEXT("("), TEXT(" "));
		map["vto"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["vto"].ReplaceInline(TEXT("["), TEXT(" "));
		map["vto"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["vto"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["vto"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["vto"].ReplaceInline(TEXT("FVector"), TEXT(" "));
		map["vto"].ReplaceInline(TEXT("X="), TEXT(" "));
		map["vto"].ReplaceInline(TEXT("Y="), TEXT(","));
		map["vto"].ReplaceInline(TEXT("Z="), TEXT(","));

		map["vto"].Trim();
		map["vto"].TrimTrailing();

		if (map["vto"].Contains("zero"))
		{
			vectorTo = FVector::ZeroVector;
		}
		else if (map["vto"].Contains("one"))
		{
			vectorTo = FVector(1, 1, 1);
		}
		else if (map["vto"].Contains("current") || map["vto"].Contains("this"))
		{
			if (actorTweening != nullptr)
			{
				vectorTo = actorTweening->GetActorLocation();
			}
			else if (componentTweening != nullptr)
			{
				vectorTo = componentTweening->GetComponentLocation();
			}
			else
			{
				UiTween::Print("You aren't performing an Actor- or SceneComponent-based operation. The 'current' and 'this' keywords only apply to those operations. Defaulting vectorTo to (0,0,0).", "error");
				vectorTo = FVector::ZeroVector;
			}
		}
		else
		{
			TArray<FString> parse;

			map["vto"].ParseIntoArray(parse, TEXT(","), true);

			vectorTo = FVector(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]));
		}

		if (map["vto"].Contains("*"))
		{
			FString left;
			FString right;
			map["vto"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			vectorTo *= FCString::Atof(*right);
		}

		UiTween::Print("vectorTo: " + vectorTo.ToString());
	}

	if (map.Contains("vector2dfrom"))
	{
		map.Add("v2from", map["vector2dfrom"]);
		map.Remove("vector2dfrom");
	}

	if (map.Contains("v2from"))
	{
		map["v2from"].ReplaceInline(TEXT("("), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT("["), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT("FVector2D"), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT("X="), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT("Y="), TEXT(","));

		map["v2from"].Trim();
		map["v2from"].TrimTrailing();

		if (map["v2from"].Contains("zero"))
		{
			vector2DFrom = FVector2D::ZeroVector;
		}
		else if (map["v2from"].Contains("one"))
		{
			vector2DFrom = FVector2D(1, 1);
		}
		else if (map["v2from"].Contains("current") || map["v2from"].Contains("this"))
		{
			if (widgetTweening != nullptr)
			{
				if (eventType == umgRTMoveFromTo)
				{
					vector2DFrom = widgetTweening->RenderTransform.Translation;
				}
				else if (eventType == umgRTScaleFromTo)
				{
					vector2DFrom = widgetTweening->RenderTransform.Scale;
				}
				else if (eventType == umgRTShearFromTo)
				{
					vector2DFrom = widgetTweening->RenderTransform.Shear;
				}
				else
				{
					UiTween::Print("You aren't performing a UMG RT Move, Scale, or Shear From/To operation. The 'current' and 'this' keywords only apply to those operations. Defaulting vector2DFrom to (0,0).", "error");
					vector2DFrom = FVector2D::ZeroVector;
				}
			}
			else if (slateTweening)
			{
				if (eventType == slateMoveFromTo)
				{
					vector2DFrom = slateTweening->TweenTransform.Translation;
				}
				else if (eventType == slateScaleFromTo)
				{
					vector2DFrom = slateTweening->TweenTransform.Scale;
				}
				else
				{
					UiTween::Print("You aren't performing a Slate Move, Scale, or Shear From/To operation. The 'current' and 'this' keywords only apply to those operations. Defaulting vector2DFrom to (0,0).", "error");
					vector2DFrom = FVector2D::ZeroVector;
				}
			}
			else
			{
				UiTween::Print("You aren't performing a Slate or UMG RT Move, Scale, or Shear From/To operation. The 'current' and 'this' keywords only apply to those operations. Defaulting vector2DFrom to (0,0).", "error");
				vector2DFrom = FVector2D::ZeroVector;
			}
		}
		else
		{
			TArray<FString> parse;

			map["v2from"].ParseIntoArray(parse, TEXT(","), true);

			vector2DFrom = FVector2D(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]));
		}

		if (map["v2from"].Contains("*"))
		{
			FString left;
			FString right;
			map["v2from"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			vector2DFrom *= FCString::Atof(*right);
		}

		UiTween::Print("vector2DFrom: " + vector2DFrom.ToString());
	}

	if (map.Contains("vector2dto"))
	{
		map.Add("v2to", map["vector2dto"]);
		map.Remove("vector2dto");
	}

	if (map.Contains("v2to"))
	{
		map["v2to"].ReplaceInline(TEXT("("), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT("["), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT("FVector2D"), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT("X="), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT("Y="), TEXT(","));

		map["v2to"].Trim();
		map["v2to"].TrimTrailing();

		if (map["v2to"].Contains("zero"))
		{
			vector2DTo = FVector2D::ZeroVector;
		}
		else if (map["v2to"].Contains("one"))
		{
			vector2DTo = FVector2D(1, 1);
		}
		else if (map["v2to"].Contains("current") || map["v2to"].Contains("this"))
		{
			if (widgetTweening != nullptr)
			{
				if (eventType == umgRTMoveFromTo)
				{
					vector2DTo = widgetTweening->RenderTransform.Translation;
				}
				else if (eventType == umgRTScaleFromTo)
				{
					vector2DTo = widgetTweening->RenderTransform.Scale;
				}
				else if (eventType == umgRTShearFromTo)
				{
					vector2DTo = widgetTweening->RenderTransform.Shear;
				}
				else
				{
					UiTween::Print("You aren't performing a UMG RT Move, Scale, or Shear From/To operation. The 'current' and 'this' keywords only apply to those operations. Defaulting vector2DTo to (0,0).", "error");
					vector2DFrom = FVector2D::ZeroVector;
				}
			}
			else if (slateTweening)
			{
				if (eventType == slateMoveFromTo)
				{
					if (slateTweening->GetRenderTransform().IsSet())
					{
						vector2DTo = slateTweening->TweenTransform.Translation;
					}
				}
				else if (eventType == slateScaleFromTo)
				{
					if (slateTweening->GetRenderTransform().IsSet())
					{
						vector2DTo = slateTweening->TweenTransform.Scale;
					}
				}
				else
				{
					UiTween::Print("You aren't performing a Slate Move, Scale, or Shear From/To operation. The 'current' and 'this' keywords only apply to those operations. Defaulting vector2DFrom to (0,0).", "error");
					vector2DFrom = FVector2D::ZeroVector;
				}
			}
			else
			{
				UiTween::Print("You aren't performing a UMG RT Move, Scale, or Shear From/To operation. The 'current' and 'this' keywords only apply to those operations. Defaulting vector2DTo to (0,0).", "error");
				vector2DFrom = FVector2D::ZeroVector;
			}
		}
		else
		{
			TArray<FString> parse;

			map["v2to"].ParseIntoArray(parse, TEXT(","), true);

			vector2DTo = FVector2D(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]));
		}

		if (map["v2to"].Contains("*"))
		{
			FString left;
			FString right;
			map["v2to"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			vector2DTo *= FCString::Atof(*right);
		}

		UiTween::Print("vector2DTo: " + vector2DTo.ToString());
	}

	if (map.Contains("rotatorfrom"))
	{
		map.Add("rfrom", map["rotatorfrom"]);
		map.Remove("rotatorfrom");
	}

	if (map.Contains("rfrom"))
	{
		map["rfrom"].ReplaceInline(TEXT("("), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT("["), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT("FRotator"), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT("P="), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT("Y="), TEXT(","));
		map["rfrom"].ReplaceInline(TEXT("R="), TEXT(","));

		map["rfrom"].Trim();
		map["rfrom"].TrimTrailing();

		if (map["rfrom"].Contains("zero"))
		{
			rotatorFrom = FRotator::ZeroRotator;
		}
		else if (map["rfrom"].Contains("one"))
		{
			rotatorFrom = FRotator(1, 1, 1);
		}
		else if (map["rfrom"].Contains("current") || map["rfrom"].Contains("this"))
		{
			if (actorTweening != nullptr)
			{
				rotatorFrom = actorTweening->GetActorRotation();
			}
			else if (componentTweening != nullptr)
			{
				rotatorFrom = componentTweening->GetComponentRotation();
			}
			else
			{
				UiTween::Print("You aren't performing an Actor- or SceneComponent-based operation. The 'current' and 'this' keywords only apply to those operations. Defaulting rotatorFrom to (0,0,0).", "error");
				rotatorFrom = FRotator::ZeroRotator;
			}
		}
		else
		{
			TArray<FString> parse;

			map["rfrom"].ParseIntoArray(parse, TEXT(","), true);

			rotatorFrom = FRotator(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]));
		}

		if (map["rfrom"].Contains("*"))
		{
			FString left;
			FString right;
			map["rfrom"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			rotatorFrom *= FCString::Atof(*right);
		}

		UiTween::Print("rotatorFrom: " + rotatorFrom.ToString());
	}

	if (map.Contains("rotatorto"))
	{
		map.Add("rto", map["rotatorto"]);
		map.Remove("rotatorto");
	}

	if (map.Contains("rto"))
	{
		map["rto"].ReplaceInline(TEXT("("), TEXT(" "));
		map["rto"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["rto"].ReplaceInline(TEXT("["), TEXT(" "));
		map["rto"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["rto"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["rto"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["rto"].ReplaceInline(TEXT("FRotator"), TEXT(" "));
		map["rto"].ReplaceInline(TEXT("P="), TEXT(" "));
		map["rto"].ReplaceInline(TEXT("Y="), TEXT(","));
		map["rto"].ReplaceInline(TEXT("R="), TEXT(","));

		map["rto"].Trim();
		map["rto"].TrimTrailing();

		if (map["rto"].Contains("zero"))
		{
			rotatorTo = FRotator::ZeroRotator;
		}
		else if (map["rto"].Contains("one"))
		{
			rotatorTo = FRotator(1, 1, 1);
		}
		else if (map["rto"].Contains("current") || map["rto"].Contains("this"))
		{
			if (actorTweening != nullptr)
			{
				rotatorTo = actorTweening->GetActorRotation();
			}
			else if (componentTweening != nullptr)
			{
				rotatorTo = componentTweening->GetComponentRotation();
			}
			else
			{
				UiTween::Print("You aren't performing an Actor- or SceneComponent-based operation. The 'current' and 'this' keywords only apply to those operations. Defaulting rotatorTo to (0,0,0).", "error");
				rotatorTo = FRotator::ZeroRotator;
			}
		}
		else
		{
			TArray<FString> parse;

			map["rto"].ParseIntoArray(parse, TEXT(","), true);

			rotatorTo = FRotator(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]));
		}

		if (map["rto"].Contains("*"))
		{
			FString left;
			FString right;
			map["rto"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			rotatorTo *= FCString::Atof(*right);
		}

		UiTween::Print("rotatorTo: " + rotatorTo.ToString());
	}

	if (map.Contains("linearcolorfrom"))
	{
		map.Add("lcfrom", map["linearcolorfrom"]);
		map.Remove("linearcolorfrom");
	}

	if (map.Contains("linearcolourfrom"))
	{
		map.Add("lcfrom", map["linearcolourfrom"]);
		map.Remove("linearcolourfrom");
	}

	if (map.Contains("colorfrom"))
	{
		map.Add("lcfrom", map["colorfrom"]);
		map.Remove("colorfrom");
	}

	if (map.Contains("colourfrom"))
	{
		map.Add("lcfrom", map["colourfrom"]);
		map.Remove("colourfrom");
	}

	if (map.Contains("cfrom"))
	{
		map.Add("lcfrom", map["cfrom"]);
		map.Remove("cfrom");
	}

	if (map.Contains("lcfrom"))
	{
		map["lcfrom"].ReplaceInline(TEXT("("), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT("["), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT("FLinearColor"), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT("R="), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT("G="), TEXT(","));
		map["lcfrom"].ReplaceInline(TEXT("B="), TEXT(","));
		map["lcfrom"].ReplaceInline(TEXT("A="), TEXT(","));

		map["lcfrom"].Trim();
		map["lcfrom"].TrimTrailing();

		TArray<FString> parse;

		map["lcfrom"].ParseIntoArray(parse, TEXT(","), true);

		if (parse.Num() == 1)
		{
			map["lcfrom"].ToLower();

			if (map["lcfrom"].Contains("black"))
			{
				linearColorFrom = FLinearColor(FColor::Black);
			}
			else if (map["lcfrom"].Contains("blue"))
			{
				linearColorFrom = FLinearColor(FColor::Blue);
			}
			else if (map["lcfrom"].Contains("cyan"))
			{
				linearColorFrom = FLinearColor(FColor::Cyan);
			}
			else if (map["lcfrom"].Contains("green"))
			{
				linearColorFrom = FLinearColor(FColor::Green);
			}
			else if (map["lcfrom"].Contains("magenta"))
			{
				linearColorFrom = FLinearColor(FColor::Magenta);
			}
			else if (map["lcfrom"].Contains("red"))
			{
				linearColorFrom = FLinearColor(FColor::Red);
			}
			else if (map["lcfrom"].Contains("white"))
			{
				linearColorFrom = FLinearColor(FColor::White);
			}
			else if (map["lcfrom"].Contains("yellow"))
			{
				linearColorFrom = FLinearColor(FColor::Yellow);
			}
		}
		else if (parse.Num() == 2)
		{
			UiTween::Print("Linear color parameters take 1, 3, or 4 arguments, not 2.", "error");
		}
		else if (parse.Num() == 3)
		{
			linearColorFrom = FLinearColor(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]));
		}
		else if (parse.Num() == 4)
		{
			linearColorFrom = FLinearColor(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]), FCString::Atof(*parse[3]));
		}

		if (map["lcfrom"].Contains("*"))
		{
			FString left;
			FString right;
			map["lcfrom"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			linearColorFrom *= FCString::Atof(*right);
			linearColorFrom.A /= FCString::Atof(*right);
		}

		UiTween::Print("linearColorFrom: " + linearColorFrom.ToString());
	}

	if (map.Contains("linearcolorto"))
	{
		map.Add("lcto", map["linearcolorto"]);
		map.Remove("linearcolorto");
	}

	if (map.Contains("linearcolourto"))
	{
		map.Add("lcto", map["linearcolourto"]);
		map.Remove("linearcolourto");
	}

	if (map.Contains("colorto"))
	{
		map.Add("lcto", map["colorto"]);
		map.Remove("colorto");
	}

	if (map.Contains("colourto"))
	{
		map.Add("lcto", map["colourto"]);
		map.Remove("colourto");
	}

	if (map.Contains("cto"))
	{
		map.Add("lcto", map["cto"]);
		map.Remove("cto");
	}

	if (map.Contains("lcto"))
	{
		map["lcto"].ReplaceInline(TEXT("("), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT("["), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT("FLinearColor"), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT("R="), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT("G="), TEXT(","));
		map["lcto"].ReplaceInline(TEXT("B="), TEXT(","));
		map["lcto"].ReplaceInline(TEXT("A="), TEXT(","));

		map["lcto"].Trim();
		map["lcto"].TrimTrailing();

		TArray<FString> parse;

		map["lcto"].ParseIntoArray(parse, TEXT(","), true);

		if (parse.Num() == 1)
		{
			map["lcto"].ToLower();

			if (map["lcto"].Contains("black"))
			{
				linearColorTo = FLinearColor(FColor::Black);
			}
			else if (map["lcto"].Contains("blue"))
			{
				linearColorTo = FLinearColor(FColor::Blue);
			}
			else if (map["lcto"].Contains("cyan"))
			{
				linearColorTo = FLinearColor(FColor::Cyan);
			}
			else if (map["lcto"].Contains("green"))
			{
				linearColorTo = FLinearColor(FColor::Green);
			}
			else if (map["lcto"].Contains("magenta"))
			{
				linearColorTo = FLinearColor(FColor::Magenta);
			}
			else if (map["lcto"].Contains("red"))
			{
				linearColorTo = FLinearColor(FColor::Red);
			}
			else if (map["lcto"].Contains("white"))
			{
				linearColorTo = FLinearColor(FColor::White);
			}
			else if (map["lcto"].Contains("yellow"))
			{
				linearColorTo = FLinearColor(FColor::Yellow);
			}
		}
		else if (parse.Num() == 2)
		{
			UiTween::Print("Color parameters take 1, 3, or 4 arguments, not 2.", "error");
		}
		else if (parse.Num() == 3)
		{
			linearColorTo = FLinearColor(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]));
		}
		else if (parse.Num() == 4)
		{
			linearColorTo = FLinearColor(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]), FCString::Atof(*parse[3]));
		}

		if (map["lcto"].Contains("*"))
		{
			FString left;
			FString right;
			map["lcto"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			linearColorTo *= FCString::Atof(*right);
			linearColorTo.A /= FCString::Atof(*right);
		}

		UiTween::Print("linearColorTo: " + linearColorTo.ToString());
	}

	if (map.Contains("delaytype"))
	{
		map.Add("dtype", map["delaytype"]);
		map.Remove("delaytype");
	}

	if (map.Contains("dtype"))
	{
		if (map["dtype"] == "first")
		{
			delayType = DelayType::first;
		}
		else if (map["dtype"] == "firstloop")
		{
			delayType = DelayType::firstLoop;
		}
		else if (map["dtype"] == "firstloopfull")
		{
			delayType = DelayType::firstLoopFull;
		}
		else if (map["dtype"] == "loop")
		{
			delayType = DelayType::loop;
		}
		else if (map["dtype"] == "loopfull")
		{
			delayType = DelayType::loopFull;
		}
		else
		{
			UiTween::Print("Acceptable parameters for the delayType argument are: first, firstloop, firstloopfull, loop, and loopfull.", "error");
		}

		UiTween::Print("delaytype: " + map["dtype"]);
	}

	if (map.Contains("delay"))
	{
		map["delay"].ReplaceInline(TEXT("("), TEXT(" "));
		map["delay"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["delay"].ReplaceInline(TEXT("["), TEXT(" "));
		map["delay"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["delay"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["delay"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["delay"].Trim();
		map["delay"].TrimTrailing();

		delay = FCString::Atof(*map["delay"]);

		if (map["delay"].Contains("*"))
		{
			FString left;
			FString right;
			map["delay"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			delay *= FCString::Atof(*right);
		}

		UiTween::Print("delay: " + FString::SanitizeFloat(delay));
	}

	if (map.Contains("ticktype"))
	{
		map.Add("ttype", map["ticktype"]);
		map.Remove("ticktype");
	}

	if (map.Contains("ttype"))
	{
		if (map["ttype"] == "seconds")
		{
			tickType = TickType::seconds;
		}
		else if (map["ttype"] == "speed")
		{
			tickType = TickType::speed;
		}
		else
		{
			UiTween::Print("Acceptable parameters for the tickType argument are: seconds and speed.", "error");
		}

		UiTween::Print("tickType: " + map["ttype"]);
	}

	if (map.Contains("ticktypevalue"))
	{
		map.Add("ttv", map["ticktypevalue"]);
		map.Remove("ticktypevalue");
	}

	if (map.Contains("ttv"))
	{
		map["ttv"].ReplaceInline(TEXT("("), TEXT(" "));
		map["ttv"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["ttv"].ReplaceInline(TEXT("["), TEXT(" "));
		map["ttv"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["ttv"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["ttv"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["ttv"].Trim();
		map["ttv"].TrimTrailing();

		tickTypeValue = FCString::Atof(*map["ttv"]);

		if (map["ttv"].Contains("*"))
		{
			FString left;
			FString right;
			map["ttv"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			tickTypeValue *= FCString::Atof(*right);
		}

		UiTween::Print("tickTypeValue: " + FString::SanitizeFloat(tickTypeValue));
	}

	if (map.Contains("speed"))
	{
		tickType = TickType::speed;

		map["speed"].ReplaceInline(TEXT("("), TEXT(" "));
		map["speed"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["speed"].ReplaceInline(TEXT("["), TEXT(" "));
		map["speed"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["speed"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["speed"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["speed"].Trim();
		map["speed"].TrimTrailing();

		tickTypeValue = FCString::Atof(*map["speed"]);

		if (map["speed"].Contains("*"))
		{
			FString left;
			FString right;
			map["speed"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			tickTypeValue *= FCString::Atof(*right);
		}

		UiTween::Print("speed: " + FString::SanitizeFloat(tickTypeValue));
	}

	if (map.Contains("timeinseconds"))
	{
		tickType = TickType::seconds;

		map["timeinseconds"].ReplaceInline(TEXT("("), TEXT(" "));
		map["timeinseconds"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["timeinseconds"].ReplaceInline(TEXT("["), TEXT(" "));
		map["timeinseconds"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["timeinseconds"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["timeinseconds"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["timeinseconds"].Trim();
		map["timeinseconds"].TrimTrailing();

		tickTypeValue = FCString::Atof(*map["timeinseconds"]);

		if (map["timeinseconds"].Contains("*"))
		{
			FString left;
			FString right;
			map["timeinseconds"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			tickTypeValue *= FCString::Atof(*right);
		}

		UiTween::Print("timeInSeconds: " + FString::SanitizeFloat(tickTypeValue));
	}

	if (map.Contains("time"))
	{
		tickType = TickType::seconds;

		map["time"].ReplaceInline(TEXT("("), TEXT(" "));
		map["time"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["time"].ReplaceInline(TEXT("["), TEXT(" "));
		map["time"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["time"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["time"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["time"].Trim();
		map["time"].TrimTrailing();

		tickTypeValue = FCString::Atof(*map["time"]);

		if (map["time"].Contains("*"))
		{
			FString left;
			FString right;
			map["time"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			tickTypeValue *= FCString::Atof(*right);
		}

		UiTween::Print("timeInSeconds: " + FString::SanitizeFloat(tickTypeValue));
	}

	if (map.Contains("punchamplitude"))
	{
		map.Add("pamp", map["punchamplitude"]);
		map.Remove("punchamplitude");
	}

	if (map.Contains("punch"))
	{
		map.Add("pamp", map["punch"]);
		map.Remove("punch");
	}

	if (map.Contains("pamp"))
	{
		map["pamp"].ReplaceInline(TEXT("("), TEXT(" "));
		map["pamp"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["pamp"].ReplaceInline(TEXT("["), TEXT(" "));
		map["pamp"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["pamp"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["pamp"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["pamp"].Trim();
		map["pamp"].TrimTrailing();

		punchAmplitude = FCString::Atof(*map["pAmp"]);

		if (map["pamp"].Contains("*"))
		{
			FString left;
			FString right;
			map["pamp"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			punchAmplitude *= FCString::Atof(*right);
		}

		UiTween::Print("punchAmplitude: " + FString::SanitizeFloat(punchAmplitude));
	}

	if (map.Contains("generatedpointdistance"))
	{
		map.Add("gpd", map["generatedpointdistance"]);
		map.Remove("generatedpointdistance");
	}

	if (map.Contains("gpd"))
	{
		map["gpd"].ReplaceInline(TEXT("("), TEXT(" "));
		map["gpd"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["gpd"].ReplaceInline(TEXT("["), TEXT(" "));
		map["gpd"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["gpd"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["gpd"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["gpd"].Trim();
		map["gpd"].TrimTrailing();

		generatedPointDistance = FCString::Atof(*map["pAmp"]);

		if (map["gpd"].Contains("*"))
		{
			FString left;
			FString right;
			map["gpd"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			generatedPointDistance *= FCString::Atof(*right);
		}

		UiTween::Print("generatedPointDistance: " + FString::SanitizeFloat(generatedPointDistance));
	}

	if (map.Contains("sweep"))
	{
		if (map["sweep"].Contains("true"))
		{
			sweep = true;
		}
		else
		{
			sweep = false;
		}

		UiTween::Print("sweep: " + map["sweep"]);
	}

	if (map.Contains("vectorconstraints"))
	{
		map.Add("vconst", map["vectorconstraints"]);
		map.Remove("vectorconstraints");
	}

	if (map.Contains("vconst"))
	{
		if (map["vconst"].Contains("no"))
		{
			vectorConstraints = VectorConstraints::none;
		}
		else if (map["vconst"] == "x")
		{
			vectorConstraints = VectorConstraints::xOnly;
		}
		else if (map["vconst"] == "y")
		{
			vectorConstraints = VectorConstraints::yOnly;
		}
		else if (map["vconst"] == "z")
		{
			vectorConstraints = VectorConstraints::zOnly;
		}
		else if (map["vconst"] == "xy")
		{
			vectorConstraints = VectorConstraints::xyOnly;
		}
		else if (map["vconst"] == "xz")
		{
			vectorConstraints = VectorConstraints::xzOnly;
		}
		else if (map["vconst"] == "yz")
		{
			vectorConstraints = VectorConstraints::yzOnly;
		}
		else
		{
			UiTween::Print("Acceptable parameters for the vectorConstraints argument are: no, none, x, y, z, xy, xz, and yz.", "error");
		}

		UiTween::Print("vectorConstraints: " + map["vconst"]);
	}

	if (map.Contains("vector2dconstraints"))
	{
		map.Add("v2const", map["vector2dconstraints"]);
		map.Remove("vector2dconstraints");
	}

	if (map.Contains("v2const"))
	{
		if (map["v2const"].Contains("no"))
		{
			vector2DConstraints = Vector2DConstraints::none;
		}
		else if (map["v2const"] == "x")
		{
			vector2DConstraints = Vector2DConstraints::xOnly;
		}
		else if (map["v2const"] == "y")
		{
			vector2DConstraints = Vector2DConstraints::yOnly;
		}
		else
		{
			UiTween::Print("Acceptable parameters for the vector2DConstraints argument are: no, none, x, and y.", "error");
		}

		UiTween::Print("vector2DConstraints: " + map["v2const"]);
	}

	if (map.Contains("rotatorconstraints"))
	{
		map.Add("rconst", map["rotatorconstraints"]);
		map.Remove("rotatorconstraints");
	}

	if (map.Contains("rconst"))
	{
		if (map["rconst"].Contains("no"))
		{
			rotatorConstraints = RotatorConstraints::none;
		}
		else if (map["rconst"] == "pitch")
		{
			rotatorConstraints = RotatorConstraints::pitchOnly;
		}
		else if (map["rconst"] == "yaw")
		{
			rotatorConstraints = RotatorConstraints::yawOnly;
		}
		else if (map["rconst"] == "roll")
		{
			rotatorConstraints = RotatorConstraints::rollOnly;
		}
		else if (map["rconst"] == "pitchyaw")
		{
			rotatorConstraints = RotatorConstraints::pitchYawOnly;
		}
		else if (map["rconst"] == "pitchroll")
		{
			rotatorConstraints = RotatorConstraints::pitchRollOnly;
		}
		else if (map["rconst"] == "yawroll")
		{
			rotatorConstraints = RotatorConstraints::yawRollOnly;
		}
		else
		{
			UiTween::Print("Acceptable parameters for the rotatorConstraints argument are: no, none, pitch, yaw, roll, pitchyaw, pitchroll, and yawroll.", "error");
		}

		UiTween::Print("rotatorConstraints: " + map["rconst"]);
	}

	if (map.Contains("looptype"))
	{
		map.Add("loop", map["looptype"]);
		map.Remove("looptype");
	}

	if (map.Contains("loop"))
	{
		if (map["loop"] == "once")
		{
			loopType = LoopType::once;
		}
		else if (map["loop"] == "pingpong" || map["loop"] == "backandforth")
		{
			loopType = LoopType::pingPong;
		}
		else if (map["loop"] == "rewind")
		{
			loopType = LoopType::rewind;
		}
		else
		{
			UiTween::Print("Acceptable parameters for the loopType argument are: once, pingpong or backandforth, and rewind.", "error");
		}

		UiTween::Print("loopType: " + map["loop"]);
	}

	if (map.Contains("looktype"))
	{
		map.Add("look", map["looktype"]);
		map.Remove("looktype");
	}

	if (map.Contains("orientation"))
	{
		map.Add("look", map["orientation"]);
		map.Remove("orientation");
	}

	if (map.Contains("look"))
	{
		if (map["look"].Contains("no"))
		{
			orientation = LookType::noOrientationChange;
		}
		else if (map["look"].Contains("path"))
		{
			orientation = LookType::orientToPath;
		}
		else if (map["look"].Contains("target"))
		{
			orientation = LookType::orientToTarget;
		}
		else
		{
			UiTween::Print("Acceptable parameters for the orientation argument are: no, path, and target.", "error");
		}

		UiTween::Print("lookType/orientation: " + map["look"]);
	}

	if (map.Contains("orientationspeed"))
	{
		map.Add("ospeed", map["orientationspeed"]);
		map.Remove("orientationspeed");
	}

	if (map.Contains("lookspeed"))
	{
		map.Add("ospeed", map["lookspeed"]);
		map.Remove("lookspeed");
	}

	if (map.Contains("ospeed"))
	{
		map["ospeed"].ReplaceInline(TEXT("("), TEXT(" "));
		map["ospeed"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["ospeed"].ReplaceInline(TEXT("["), TEXT(" "));
		map["ospeed"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["ospeed"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["ospeed"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["ospeed"].Trim();
		map["ospeed"].TrimTrailing();

		orientationSpeed = FCString::Atof(*map["ospeed"]);

		if (map["ospeed"].Contains("*"))
		{
			FString left;
			FString right;
			map["ospeed"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			orientationSpeed *= FCString::Atof(*right);
		}

		UiTween::Print("orientationSpeed: " + FString::SanitizeFloat(orientationSpeed));
	}

	if (map.Contains("tickwhenpaused"))
	{
		map.Add("twp", map["tickwhenpaused"]);
		map.Remove("tickwhenpaused");
	}

	if (map.Contains("twp"))
	{
		if (map["twp"].Contains("true"))
		{
			tickWhenPaused = true;
		}
		else
		{
			tickWhenPaused = false;
		}

		UiTween::Print("tickWhenPaused: " + map["twp"]);
	}

	if (map.Contains("ignoretimedilation"))
	{
		map.Add("itd", map["ignoretimedilation"]);
		map.Remove("ignoretimedilation");
	}

	if (map.Contains("itd"))
	{
		if (map["itd"].Contains("true"))
		{
			ignoreTimeDilation = true;
		}
		else
		{
			ignoreTimeDilation = false;
		}

		UiTween::Print("ignoreTimeDilation: " + map["itd"]);
	}

	if (map.Contains("onstarttarget"))
	{
		map.Add("ost", map["onstarttarget"]);
		map.Remove("onstarttarget");
	}

	if (map.Contains("ontweenstarttarget"))
	{
		map.Add("ost", map["ontweenstarttarget"]);
		map.Remove("ontweenstarttarget");
	}

	if (map.Contains("ost"))
	{
		onTweenStartTarget = UiTween::FindObjectByName(map["ost"]);
		UiTween::Print(onTweenStartTarget->GetFName().ToString());
	}

	if (map.Contains("onticktarget"))
	{
		map.Add("ott", map["onticktarget"]);
		map.Remove("onticktarget");
	}

	if (map.Contains("ontweenupdatetarget"))
	{
		map.Add("ott", map["ontweenupdatetarget"]);
		map.Remove("ontweenupdatetarget");
	}

	if (map.Contains("ott"))
	{
		onTweenUpdateTarget = UiTween::FindObjectByName(map["ott"]);
		UiTween::Print(onTweenUpdateTarget->GetFName().ToString());
	}

	if (map.Contains("onlooptarget"))
	{
		map.Add("olt", map["onlooptarget"]);
		map.Remove("onlooptarget");
	}

	if (map.Contains("ontweenlooptarget"))
	{
		map.Add("olt", map["ontweenlooptarget"]);
		map.Remove("ontweenlooptarget");
	}

	if (map.Contains("olt"))
	{
		onTweenLoopTarget = UiTween::FindObjectByName(map["olt"]);
		UiTween::Print(onTweenLoopTarget->GetFName().ToString());
	}

	if (map.Contains("oncompletetarget"))
	{
		map.Add("oct", map["oncompletetarget"]);
		map.Remove("oncompletetarget");
	}

	if (map.Contains("ontweencompletetarget"))
	{
		map.Add("oct", map["ontweencompletetarget"]);
		map.Remove("ontweencompletetarget");
	}

	if (map.Contains("oct"))
	{
		onTweenCompleteTarget = UiTween::FindObjectByName(map["oct"]);
		UiTween::Print(onTweenCompleteTarget->GetFName().ToString());
	}

	if (map.Contains("orientationtarget"))
	{
		map.Add("ot", map["orientationtarget"]);
		map.Remove("orientationtarget");
	}

	if (map.Contains("ot"))
	{
		orientationTarget = (AActor*)UiTween::FindObjectByName(map["ot"]);
		UiTween::Print(orientationTarget->GetFName().ToString());
	}

	if (map.Contains("splinecomponent"))
	{
		map.Add("spline", map["splinecomponent"]);
		map.Remove("splinecomponent");
	}

	if (map.Contains("spline"))
	{
		UObject* o = UiTween::FindObjectByName(map["spline"]);

		if (o->IsA(USplineComponent::StaticClass()))
		{
			splineComponent = (USplineComponent*)o;
		}

		UiTween::Print(splineComponent->GetFName().ToString());
	}

	if (map.Contains("interpolatetospline"))
	{
		map.Add("its", map["interpolatetospline"]);
		map.Remove("interpolatetospline");
	}

	if (map.Contains("its"))
	{
		if (map["its"].Contains("true"))
		{
			interpolateToSpline = true;
		}
		else
		{
			interpolateToSpline = false;
		}

		UiTween::Print("interpolateToSpline: " + map["its"]);
	}

	if (map.Contains("switchpathorientationdirection"))
	{
		map.Add("spod", map["switchpathorientationdirection"]);
		map.Remove("switchpathorientationdirection");
	}

	if (map.Contains("spod"))
	{
		if (map["spod"].Contains("true"))
		{
			switchPathOrientationDirection = true;
		}
		else
		{
			switchPathOrientationDirection = false;
		}

		UiTween::Print("switchPathOrientationDirection: " + map["spod"]);
	}

	if (map.Contains("destroysplineobject"))
	{
		map.Add("dso", map["destroysplineobject"]);
		map.Remove("destroysplineobject");
	}

	if (map.Contains("destroyspline"))
	{
		map.Add("dso", map["destroyspline"]);
		map.Remove("destroyspline");
	}

	if (map.Contains("dso"))
	{
		if (map["dso"].Contains("true"))
		{
			destroySplineObject = true;
		}
		else
		{
			destroySplineObject = false;
		}

		UiTween::Print("destroySplineObject: " + map["dso"]);
	}

	if (map.Contains("shortestpath"))
	{
		map.Add("short", map["shortestpath"]);
		map.Remove(map["shortestpath"]);
	}

	if (map.Contains("short"))
	{
		if (map["short"].Contains("true"))
		{
			shortestPath = true;
		}
		else
		{
			shortestPath = false;
		}

		UiTween::Print("shortestPath: " + map["short"]);
	}

	if (map.Contains("easetype"))
	{
		map.Add("ease", map["easetype"]);
		map.Remove(map["easetype"]);
	}

	if (map.Contains("ease"))
	{
		map["ease"].Trim();
		map["ease"].TrimTrailing();

		if (map["ease"].Contains("linear"))
		{
			easeType = EaseType::linear;
		}
		else if (map["ease"].Contains("punch"))
		{
			easeType = EaseType::punch;
		}
		else if (map["ease"].Contains("spring"))
		{
			easeType = EaseType::spring;
		}
		else if (map["ease"].Contains("curve"))
		{
			easeType = EaseType::customCurve;
		}
		else if (map["ease"].Contains("easeInAndOutBack") || map["ease"].Contains("ioback"))
		{
			easeType = EaseType::easeInAndOutBack;
		}
		else if (map["ease"].Contains("easeInAndOutBounce") || map["ease"].Contains("iobounce"))
		{
			easeType = EaseType::easeInAndOutBounce;
		}
		else if (map["ease"].Contains("easeInAndOutCircular") || map["ease"].Contains("iocirc"))
		{
			easeType = EaseType::easeInAndOutCircular;
		}
		else if (map["ease"].Contains("easeInAndOutCubic") || map["ease"].Contains("iocubic"))
		{
			easeType = EaseType::easeInAndOutCubic;
		}
		else if (map["ease"].Contains("easeInAndOutElastic") || map["ease"].Contains("ioelas"))
		{
			easeType = EaseType::easeInAndOutElastic;
		}
		else if (map["ease"].Contains("easeInAndOutExponential") || map["ease"].Contains("ioexpo"))
		{
			easeType = EaseType::easeInAndOutExponential;
		}
		else if (map["ease"].Contains("easeInAndOutQuadratic") || map["ease"].Contains("ioquad"))
		{
			easeType = EaseType::easeInAndOutQuadratic;
		}
		else if (map["ease"].Contains("easeInAndOutQuartic") || map["ease"].Contains("ioquart"))
		{
			easeType = EaseType::easeInAndOutQuartic;
		}
		else if (map["ease"].Contains("easeInAndOutQuintic") || map["ease"].Contains("ioquint"))
		{
			easeType = EaseType::easeInAndOutQuintic;
		}
		else if (map["ease"].Contains("easeInAndOutSine") || map["ease"].Contains("iosine"))
		{
			easeType = EaseType::easeInAndOutSine;
		}
		else if (map["ease"].Contains("easeInBack") || map["ease"].Contains("iback"))
		{
			easeType = EaseType::easeInBack;
		}
		else if (map["ease"].Contains("easeInBounce") || map["ease"].Contains("ibounce"))
		{
			easeType = EaseType::easeInBounce;
		}
		else if (map["ease"].Contains("easeInCircular") || map["ease"].Contains("icirc"))
		{
			easeType = EaseType::easeInCircular;
		}
		else if (map["ease"].Contains("easeInCubic") || map["ease"].Contains("icubic"))
		{
			easeType = EaseType::easeInCubic;
		}
		else if (map["ease"].Contains("easeInElastic") || map["ease"].Contains("ielas"))
		{
			easeType = EaseType::easeInElastic;
		}
		else if (map["ease"].Contains("easeInExponential") || map["ease"].Contains("iexpo"))
		{
			easeType = EaseType::easeInExponential;
		}
		else if (map["ease"].Contains("easeInQuadratic") || map["ease"].Contains("iquad"))
		{
			easeType = EaseType::easeInQuadratic;
		}
		else if (map["ease"].Contains("easeInQuartic") || map["ease"].Contains("iquart"))
		{
			easeType = EaseType::easeInQuartic;
		}
		else if (map["ease"].Contains("easeInQuintic") || map["ease"].Contains("iquint"))
		{
			easeType = EaseType::easeInQuintic;
		}
		else if (map["ease"].Contains("easeInSine") || map["ease"].Contains("isine"))
		{
			easeType = EaseType::easeInSine;
		}
		else if (map["ease"].Contains("easeOutBack") || map["ease"].Contains("oback"))
		{
			easeType = EaseType::easeOutBack;
		}
		else if (map["ease"].Contains("easeOutBounce") || map["ease"].Contains("obounce"))
		{
			easeType = EaseType::easeOutBounce;
		}
		else if (map["ease"].Contains("easeOutCircular") || map["ease"].Contains("ocirc"))
		{
			easeType = EaseType::easeOutCircular;
		}
		else if (map["ease"].Contains("easeOutCubic") || map["ease"].Contains("ocubic"))
		{
			easeType = EaseType::easeOutCubic;
		}
		else if (map["ease"].Contains("easeOutElastic") || map["ease"].Contains("oelas"))
		{
			easeType = EaseType::easeOutElastic;
		}
		else if (map["ease"].Contains("easeOutExponential") || map["ease"].Contains("oexpo"))
		{
			easeType = EaseType::easeOutExponential;
		}
		else if (map["ease"].Contains("easeOutQuadratic") || map["ease"].Contains("oquad"))
		{
			easeType = EaseType::easeOutQuadratic;
		}
		else if (map["ease"].Contains("easeOutQuartic") || map["ease"].Contains("oquart"))
		{
			easeType = EaseType::easeOutQuartic;
		}
		else if (map["ease"].Contains("easeOutQuintic") || map["ease"].Contains("oquint"))
		{
			easeType = EaseType::easeOutQuintic;
		}
		else if (map["ease"].Contains("easeOutSine") || map["ease"].Contains("osine"))
		{
			easeType = EaseType::easeOutSine;
		}

		UiTween::Print("easeType: " + map["ease"]);
	}

	if (map.Contains("timerinterval"))
	{
		map.Add("ti", map["timerinterval"]);
		map.Remove(map["timerinterval"]);
	}

	if (map.Contains("interval"))
	{
		map.Add("ti", map["interval"]);
		map.Remove(map["interval"]);
	}

	if (map.Contains("ti"))
	{
		map["ti"].ReplaceInline(TEXT("("), TEXT(" "));
		map["ti"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["ti"].ReplaceInline(TEXT("["), TEXT(" "));
		map["ti"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["ti"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["ti"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["ti"].Trim();
		map["ti"].TrimTrailing();

		timerInterval = FCString::Atof(*map["ti"]);

		if (map["ti"].Contains("*"))
		{
			FString left;
			FString right;
			map["ti"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			timerInterval *= FCString::Atof(*right);
		}

		if (map["ti"].Contains("fps"))
		{
			timerInterval = 1 / timerInterval;
		}

		UiTween::Print("timerInterval: " + FString::SanitizeFloat(timerInterval));
	}

	if (map.Contains("tietoobjectvalidity"))
	{
		map.Add("tov", map["tietoobjectvalidity"]);
		map.Remove(map["tietoobjectvalidity"]);
	}

	if (map.Contains("tov"))
	{
		if (map["tov"].Contains("true"))
		{
			tieToObjectValidity = true;
		}
		else
		{
			tieToObjectValidity = false;
		}

		UiTween::Print("tieToObjectValidity: " + map["tov"]);
	}

	if (map.Contains("enforcevalueto"))
	{
		map.Add("evt", map["enforcevalueto"]);
		map.Remove(map["enforcevalueto"]);
	}

	if (map.Contains("evt"))
	{
		if (map["evt"].Contains("true"))
		{
			enforceValueTo = true;
		}
		else
		{
			enforceValueTo = false;
		}

		UiTween::Print("enforceValueTo: " + map["evt"]);
	}

	if (map.Contains("maximumloopsections"))
	{
		map.Add("mls", map["maximumloopsections"]);
		map.Remove(map["maximumloopsections"]);
	}

	if (map.Contains("maxloops"))
	{
		map.Add("mls", map["maxloops"]);
		map.Remove(map["maxloops"]);
	}

	if (map.Contains("mls"))
	{
		map["mls"].ReplaceInline(TEXT("("), TEXT(" "));
		map["mls"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["mls"].ReplaceInline(TEXT("["), TEXT(" "));
		map["mls"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["mls"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["mls"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["mls"].Trim();
		map["mls"].TrimTrailing();

		maximumLoopSections = FCString::Atoi(*map["mls"]);

		if (map["mls"].Contains("*"))
		{
			FString left;
			FString right;
			map["mls"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			maximumLoopSections *= FCString::Atoi(*right);
		}

		UiTween::Print("maximumLoopSections: " + FString::FromInt(maximumLoopSections));
	}
}

void AiTweenEvent::IgnorePauseTimeDilationSettings()
{
	AActor* actor;
	FString boo;

	if (eventType == actorMoveFromTo || eventType == actorRotateFromTo || eventType == actorScaleFromTo || eventType == actorMoveToSplinePoint || eventType == actorRotateToSplinePoint)
	{
		if (actorTweening)
		{
			actor = actorTweening;
		}
		else
		{
			actor = this;
		}
	}
	else if (eventType == compMoveFromTo || eventType == compRotateFromTo || eventType == compScaleFromTo || eventType == compMoveToSplinePoint || eventType == compRotateToSplinePoint)
	{
		if (componentTweening)
		{
			actor = componentTweening->GetOwner();
		}
		else
		{
			actor = this;
		}
	}
	else
	{
		actor = this;
	}

	if (tickWhenPaused)
	{
		//Needs to be able to get whether actor is tickable when paused
		actorTickableWhenPaused = actor->PrimaryActorTick.bTickEvenWhenPaused;
		this->SetTickableWhenPaused(tickWhenPaused);
		actor->SetTickableWhenPaused(tickWhenPaused);
	}

	if (actor->PrimaryActorTick.bTickEvenWhenPaused)
	{
		boo = "PrmaryActorTick.bTickEvenWhenPaused = true";
	}
	else
	{
		boo = "PrmaryActorTick.bTickEvenWhenPaused = false";
	}
	UiTween::Print(boo);

	if (ignoreTimeDilation)
	{
		actorTimeDilation = actor->CustomTimeDilation;
		this->CustomTimeDilation = actor->CustomTimeDilation = (1 / GetWorldSettings()->TimeDilation);
	}
}

void AiTweenEvent::RestorePauseTimeDilationSettings()
{
	AActor* actor;
	FString boo;

	if (eventType == actorMoveFromTo || eventType == actorRotateFromTo || eventType == actorScaleFromTo || eventType == actorMoveToSplinePoint || eventType == actorRotateToSplinePoint)
	{
		if (actorTweening)
		{
			actor = actorTweening;
		}
		else
		{
			actor = this;
		}
	}
	else if (eventType == compMoveFromTo || eventType == compRotateFromTo || eventType == compScaleFromTo || eventType == compMoveToSplinePoint || eventType == compRotateToSplinePoint)
	{
		if (componentTweening)
		{
			actor = componentTweening->GetOwner();
		}
		else
		{
			actor = this;
		}
	}
	else
	{
		actor = this;
	}

	if (tickWhenPaused)
	{
		//Resets actor back to not tickable when paused for now until we can get whether an actor is tickable when paused
		actor->SetTickableWhenPaused(actorTickableWhenPaused);
	}

	if (actor->PrimaryActorTick.bTickEvenWhenPaused)
	{
		boo = "PrmaryActorTick.bTickEvenWhenPaused = true";
	}
	else
	{
		boo = "PrmaryActorTick.bTickEvenWhenPaused = false";
	}
	UiTween::Print(boo);

	if (ignoreTimeDilation)
	{
		actor->CustomTimeDilation = actorTimeDilation;
	}
}

void AiTweenEvent::SetTickTypeValue()
{
	if (tickType != seconds)
	{
		if (eventType == actorMoveFromTo || eventType == actorScaleFromTo || eventType == compMoveFromTo || eventType == compScaleFromTo || eventType == vectorFromTo)
		{
			tickTypeValue = ((UiTween::GetDistanceBetweenTwoVectors(vectorFrom, vectorTo) / tickTypeValue) / GetWorldSettings()->WorldToMeters);
		}
		else if (eventType == actorRotateFromTo || eventType == compRotateFromTo || eventType == rotatorFromTo)
		{
			tickTypeValue = ((UiTween::GetDistanceBetweenTwoVectors(rotatorFrom.Vector(), rotatorTo.Vector()) / tickTypeValue) / GetWorldSettings()->WorldToMeters);
		}
		else if (eventType == actorMoveToSplinePoint || eventType == actorRotateToSplinePoint || eventType == compMoveToSplinePoint || eventType == compRotateToSplinePoint)
		{
			tickTypeValue = (FMath::Abs(0 - splineComponent->Duration) / tickTypeValue);
		}
		else if (eventType == umgRTMoveFromTo || eventType == umgRTScaleFromTo || eventType == umgRTShearFromTo || eventType == vector2DFromTo)
		{
			tickTypeValue = ((UiTween::GetDistanceBetweenTwoVectors(FVector(vector2DFrom.X, vector2DFrom.Y, 0), FVector(vector2DTo.X, vector2DTo.Y, 0)) / tickTypeValue) / GetWorldSettings()->WorldToMeters);
		}
		else if (eventType == floatFromTo || eventType == umgRTRotateFromTo)
		{
			tickTypeValue = (FMath::Abs(floatFrom - floatTo) / tickTypeValue);
		}
		else if (eventType == linearColorFromTo)
		{
			tickTypeValue = ((UiTween::GetDistanceBetweenTwoVectors(FVector(linearColorFrom.R, linearColorFrom.G, linearColorFrom.B), FVector(linearColorTo.R, linearColorTo.G, linearColorTo.B)) / tickTypeValue) / GetWorldSettings()->WorldToMeters);
		}
	}
}

float AiTweenEvent::GetAlphaFromEquation(float value)
{
	//Big thanks to Bob Berkebile at PixelPlacement for the original iTween equation interpretations. Unlike the rest of 
	//iTween for UE4, Easetype equations are taken almost 100% verbatim from iTween source code (with Bob's permission, 
	//of course).

	//TERMS OF USE - EASING EQUATIONS Open source under the BSD License. Copyright(c)2001 Robert PennerAll rights reserved. 

	//Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
	//following conditions are met : Redistributions of source code must retain the above copyright notice, this list 
	//of conditions and the following disclaimer. Redistributions in binary form must reproduce the above copyright 
	//notice, this list of conditions and the following disclaimer in the documentation and / or other materials 
	//provided with the distribution. Neither the name of the author nor the names of contributors may be used to endorse 
	//or promote products derived from this software without specific prior written permission. THIS SOFTWARE IS PROVIDED 
	//BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
	//TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
	//THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
	//CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
	//OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
	//LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
	//ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	switch (easeType)
	{

	case linear:
		return value;

	case spring:
	{
				   value = FMath::Clamp<float>(value, start, end);
				   value = (FMath::Sin(value * pi * (0.2f + 2.5f * value * value * value)) * FMath::Pow(1.f - value, 2.2f) + value) * (1.f + (1.2f * (1.f - value)));
				   return start + (end - start) * value;
	}


	case easeInQuadratic:
	{
							end -= start;
							return end * value * value + start;
	}


	case easeOutQuadratic:
	{
							 end -= start;
							 return -end * value * (value - 2) + start;
	}


	case easeInAndOutQuadratic:
	{
								  value /= 0.5f;
								  end -= start;
								  if (value < 1) return end / 2 * value * value + start;
								  value--;
								  return -end / 2 * (value * (value - 2) - 1) + start;
	}


	case easeInCubic:
	{
						end -= start;
						return end * value * value * value + start;
	}


	case easeOutCubic:
	{
						 value--;
						 end -= start;
						 return end * (value * value * value + 1) + start;
	}


	case easeInAndOutCubic:
	{
							  value /= 0.5f;
							  end -= start;
							  if (value < 1) return end / 2 * value * value * value + start;
							  value -= 2;
							  return end / 2 * (value * value * value + 2) + start;
	}


	case easeInQuartic:
	{
						  end -= start;
						  return end * value * value * value * value + start;
	}


	case easeOutQuartic:
	{
						   value--;
						   end -= start;
						   return -end * (value * value * value * value - 1) + start;
	}


	case easeInAndOutQuartic:
	{
								value /= 0.5f;
								end -= start;
								if (value < 1) return end / 2 * value * value * value * value + start;
								value -= 2;
								return -end / 2 * (value * value * value * value - 2) + start;
	}


	case easeInQuintic:
	{
						  end -= start;
						  return end * value * value * value * value * value + start;
	}


	case easeOutQuintic:
	{
						   value--;
						   end -= start;
						   return end * (value * value * value * value * value + 1) + start;
	}


	case easeInAndOutQuintic:
	{
								value /= 0.5f;
								end -= start;
								if (value < 1) return end / 2 * value * value * value * value * value + start;
								value -= 2;
								return end / 2 * (value * value * value * value * value + 2) + start;
	}


	case easeInSine:
	{
					   end -= start;
					   return -end * FMath::Cos(value / 1 * (pi / 2)) + end + start;
	}


	case easeOutSine:
	{
						end -= start;
						return end * FMath::Sin(value / 1 * (pi / 2)) + start;
	}


	case easeInAndOutSine:
	{
							 end -= start;
							 return -end / 2 * (FMath::Cos(pi * value / 1) - 1) + start;
	}


	case easeInExponential:
	{
							  end -= start;
							  return end * FMath::Pow(2, 10 * (value / 1 - 1)) + start;
	}


	case easeOutExponential:
	{
							   end -= start;
							   return end * (-FMath::Pow(2, -10 * value / 1) + 1) + start;
	}


	case easeInAndOutExponential:
	{
									value /= 0.5f;
									end -= start;
									if (value < 1) return end / 2 * FMath::Pow(2, 10 * (value - 1)) + start;
									value--;
									return end / 2 * (-FMath::Pow(2, -10 * value) + 2) + start;
	}


	case easeInCircular:
	{
						   end -= start;
						   return -end * (FMath::Sqrt(1 - value * value) - 1) + start;
	}


	case easeOutCircular:
	{
							value--;
							end -= start;
							return end * FMath::Sqrt(1 - value * value) + start;
	}


	case easeInAndOutCircular:
	{
								 value /= 0.5f;
								 end -= start;
								 if (value < 1) return -end / 2 * (FMath::Sqrt(1 - value * value) - 1) + start;
								 value -= 2;
								 return end / 2 * (FMath::Sqrt(1 - value * value) + 1) + start;
	}


	case easeInBounce:
		return EaseInBounce(value);

	case easeOutBounce:
		return EaseOutBounce(value);


	case easeInAndOutBounce:
		return EaseInOutBounce(value);

	case easeInBack:
	{
					   end -= start;
					   value /= 1;
					   float s = 1.70158f;
					   return end * (value)* value * ((s + 1) * value - s) + start;
	}


	case easeOutBack:
	{
						float s = 1.70158f;
						end -= start;
						value = (value / 1) - 1;
						return end * ((value)* value * ((s + 1) * value + s) + 1) + start;
	}


	case easeInAndOutBack:
	{
							 float s = 1.70158f;
							 end -= start;
							 value /= 0.5f;
							 if ((value) < 1){
								 s *= (1.525f);
								 return end / 2 * (value * value * (((s)+1) * value - s)) + start;
							 }
							 value -= 2;
							 s *= (1.525f);
							 return end / 2 * ((value)* value * (((s)+1) * value + s) + 2) + start;
	}


	case punch:
	{
				  float s = 9;
				  if (value == 0){
					  return 0;
				  }
				  if (value == 1){
					  return 0;
				  }
				  float period = 1 * 0.3f;
				  s = period / (2 * pi) * FMath::Asin(0);
				  return (punchAmplitude * FMath::Pow(2, -10 * value) * FMath::Sin((value * 1 - s) * (2 * pi) / period));
	}


	case easeInElastic:
	{
						  end -= start;

						  float d = 1.f;
						  float p = d * 0.3f;
						  float s = 0;
						  float a = 0;

						  if (value == 0) return start;

						  if ((value /= d) == 1) return start + end;

						  if (a == 0.f || a < FMath::Abs(end)){
							  a = end;
							  s = p / 4;
						  }
						  else{
							  s = p / (2 * pi) * FMath::Asin(end / a);
						  }
						  value -= 1;
						  return -(a * FMath::Pow(2, 10 * (value)) * FMath::Sin((value * d - s) * (2 * pi) / p)) + start;
	}


	case easeOutElastic:
		//Thank you to rafael.marteleto for fixing this as a port over from Pedro's UnityTween
	{
						   end -= start;

						   float d = 1.f;
						   float p = d * 0.3f;
						   float s = 0;
						   float a = 0;

						   if (value == 0) return start;

						   if ((value /= d) == 1) return start + end;

						   if (a == 0.f || a < FMath::Abs(end)){
							   a = end;
							   s = p / 4;
						   }
						   else{
							   s = p / (2 * pi) * FMath::Asin(end / a);
						   }

						   return (a * FMath::Pow(2, -10 * value) * FMath::Sin((value * d - s) * (2 * pi) / p) + end + start);
	}


	case easeInAndOutElastic:
	{
								end -= start;

								float d = 1.f;
								float p = d * 0.3f;
								float s = 0;
								float a = 0;

								if (value == 0) return start;

								if ((value /= d / 2) == 2) return start + end;

								if (a == 0.f || a < FMath::Abs(end)){
									a = end;
									s = p / 4;
								}
								else{
									s = p / (2 * pi) * FMath::Asin(end / a);
								}

								//Causes problems when building for iOS. Uncomment this to restore a working EaseInAndOutElastic for non-iOS. 
								//I figured that more people would prefer to be able to build their iOS games than to use this one
								//infrequently used EaseType. This is being evaluated for improvement.
								/*if (value < 1) return -0.5f * (a * FMath::Pow(2, 10 * (value -= 1)) * FMath::Sin((value * d - s) * (2 * pi) / p)) + start;
								return a * FMath::Pow(2, -10 * (value -= 1)) * FMath::Sin((value * d - s) * (2 * pi) / p) * 0.5f + end + start;*/
								value = value - 1;
								if (value < 1) return -0.5f * (a * FMath::Pow(2, 10 * (value)) * FMath::Sin((value * d - s) * (2 * pi) / p)) + start;
								return a * FMath::Pow(2, -10 * (value)) * FMath::Sin((value * d - s) * (2 * pi) / p) * 0.5f + end + start;
	}

	case customCurve:
	{
						if (customEaseTypeCurve)
						{
							return customEaseTypeCurve->GetFloatValue(value);
						}
						else
						{
							return value;
						}
	}
	default:
		return value;
	}
}

float AiTweenEvent::EaseInBounce(float value)
{
	end -= start;
	float d = 1.f;
	return end - EaseOutBounce(d - value) + start;
}

float AiTweenEvent::EaseOutBounce(float value)
{
	value /= 1.f;
	end -= start;
	if (value < (1 / 2.75f)){
		return end * (7.5625f * value * value) + start;
	}
	else if (value < (2 / 2.75f)){
		value -= (1.5f / 2.75f);
		return end * (7.5625f * (value)* value + 0.75f) + start;
	}
	else if (value < (2.5 / 2.75)){
		value -= (2.25f / 2.75f);
		return end * (7.5625f * (value)* value + 0.9375f) + start;
	}
	else{
		value -= (2.625f / 2.75f);
		return end * (7.5625f * (value)* value + 0.984375f) + start;
	}
}

float AiTweenEvent::EaseInOutBounce(float value)
{
	end -= start;
	float d = 1.f;
	if (value < d / 2)
	{
		return EaseInBounce(value *= 2) * 0.5f + start;
	}
	else
	{
		return EaseOutBounce(value * 2 - d) * 0.5f + end*0.5f + start;
	}
}

void AiTweenEvent::EndPhase()
{
	shouldTween = false;
	shouldTick = false;

	//OnTweenComplete Interface Message
	RunInterface(onTweenCompleteTarget, "complete");
	RunSlateInterface(onTweenCompleteSlateTarget, "complete");

	RestorePauseTimeDilationSettings();

	if (splineComponent && destroySplineObject)
	{
		splineComponent->GetOwner()->Destroy();
	}

	this->Destroy();
}

void AiTweenEvent::DelayTween()
{
	if (delayCount < delay)
	{
		delayCount += deltaSeconds;
	}
	else
	{
		if (!shouldTween)
		{
			FirstSet();

			//OnTweenStart Interface Message
			RunInterface(onTweenStartTarget, "start");
			RunSlateInterface(onTweenStartSlateTarget, "start");

			shouldTween = true;

			ExecuteTween();
		}
	}
}

void AiTweenEvent::LoopSectionStuff()
{
	numberOfLoopSections++;

	if (maximumLoopSections != 0)
	{
		if (maximumLoopSections < 0)
		{
			maximumLoopSections *= -1;
		}
		if (numberOfLoopSections >= maximumLoopSections)
		{
			EndPhase();
		}
	}

	//OnTweenLoop Interface Message
	RunInterface(onTweenLoopTarget, "loop");
	RunSlateInterface(onTweenLoopSlateTarget, "loop");
}

void AiTweenEvent::LoopWithDelay()
{
	shouldTween = false;
	shouldTick = false;
	SetTickTypeValue();
	alpha = 0.f;
	delayCount = 0;
	shouldDelay = true;
	shouldTick = true;
}

void AiTweenEvent::LoopWithoutDelay()
{
	shouldTween = false;
	shouldTick = false;
	SetTickTypeValue();
	alpha = 0.f;
	shouldTick = true;
	shouldTween = true;
}

void AiTweenEvent::SpacializeValues()
{
	if (eventType == actorMoveFromTo)
	{
		if (coordinateSpace == self)
		{
			vectorTo = (actorTweening->GetActorRotation().RotateVector(vectorTo) + actorTweening->GetActorLocation());
		}
		else if (coordinateSpace == parent)
		{
			if (actorTweening->GetRootComponent()->GetAttachParent())
			{
				vectorTo = (actorTweening->GetRootComponent()->GetAttachParent()->GetOwner()->GetActorRotation().RotateVector(vectorTo) + actorTweening->GetRootComponent()->GetAttachParent()->GetOwner()->GetActorLocation());
			}
		}
	}
	else if (eventType == actorRotateFromTo)
	{
		if (coordinateSpace == self)
		{
			rotatorTo = FRotator(FQuat(actorTweening->GetActorRotation()) * FQuat(rotatorTo));
		}
		else if (coordinateSpace == parent)
		{
			if (actorTweening->GetRootComponent()->GetAttachParent())
			{
				rotatorTo = FRotator(FQuat(actorTweening->GetRootComponent()->GetAttachParent()->GetOwner()->GetActorRotation()) * FQuat(rotatorTo));

			}
		}
	}
	else if (eventType == actorScaleFromTo)
	{
		if (coordinateSpace == parent)
		{
			vectorTo = actorTweening->GetRootComponent()->GetAttachParent()->GetComponentScale() * vectorTo;
		}
	}
	else if (eventType == compMoveFromTo)
	{
		if (coordinateSpace == self)
		{
			vectorTo = (componentTweening->GetComponentRotation().RotateVector(vectorTo) + componentTweening->GetComponentLocation());
		}
		else if (coordinateSpace == parent)
		{
			if (componentTweening->GetOwner())
			{
				vectorTo = (componentTweening->GetOwner()->GetActorRotation().RotateVector(vectorTo) + componentTweening->GetOwner()->GetActorLocation());
			}
		}
	}
	else if (eventType == compRotateFromTo)
	{
		if (coordinateSpace == self)
		{
			rotatorTo = FRotator(FQuat(componentTweening->GetComponentRotation()) * FQuat(rotatorTo));
		}
		else if (coordinateSpace == parent)
		{
			if (componentTweening->GetOwner())
			{
				rotatorTo = FRotator(FQuat(componentTweening->GetOwner()->GetActorRotation()) * FQuat(rotatorTo));
			}
		}
	}
	else if (eventType == compScaleFromTo)
	{
		if (coordinateSpace == parent)
		{
			vectorTo = componentTweening->GetOwner()->GetActorScale3D() * vectorTo;
		}
	}
}

void AiTweenEvent::SwitchValues()
{
	if (eventType == actorMoveFromTo || eventType == actorScaleFromTo || eventType == compMoveFromTo || eventType == compScaleFromTo || eventType == vectorFromTo)
	{
		vectorTemp = vectorFrom;
		vectorFrom = vectorTo;
		vectorTo = vectorTemp;
	}
	else if (eventType == actorRotateFromTo || eventType == compRotateFromTo || eventType == rotatorFromTo)
	{
		rotatorTemp = rotatorFrom;
		rotatorFrom = rotatorTo;
		rotatorTo = rotatorTemp;
	}
	else if (eventType == umgRTMoveFromTo || eventType == umgRTScaleFromTo || eventType == umgRTShearFromTo || eventType == vector2DFromTo || eventType == slateMoveFromTo || eventType == slateScaleFromTo)
	{
		vector2DTemp = vector2DFrom;
		vector2DFrom = vector2DTo;
		vector2DTo = vector2DTemp;
	}
	else if (eventType == floatFromTo || eventType == umgRTRotateFromTo)
	{
		floatTemp = floatFrom;
		floatFrom = floatTo;
		floatTo = floatTemp;
	}
	else if (eventType == linearColorFromTo)
	{
		linearColorTemp = linearColorFrom;
		linearColorFrom = linearColorTo;
		linearColorTo = linearColorTemp;
	}
	else
	{

	}
}

FString AiTweenEvent::GetEventTypeEnumAsFString(EEventType::EventType enumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEventType"), true);

	if (!EnumPtr)
	{
		return FString("Invalid");
	}

	return EnumPtr->GetEnumName(enumValue);
}

bool AiTweenEvent::IsEventDataType()
{
	if (eventType == floatFromTo || eventType == linearColorFromTo || eventType == vectorFromTo || eventType == vector2DFromTo || eventType == rotatorFromTo)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AiTweenEvent::RunInterface(UObject* target, FString type)
{
    /* Little hack to work with c++
	IiTInterface* interf = Cast<IiTInterface>(target);

	if (interf)
	{
		if (type == "start")
		{
			interf->OnTweenStart(this, actorTweening, componentTweening, widgetTweening, tweenName);
		}
		else if (type == "update")
		{
			//interf->OnTweenUpdate(this, actorTweening, componentTweening, widgetTweening, tweenName, dtv, alpha);

			if (IsEventDataType())
			{
				interf->OnTweenDataTick(this, tweenName, dtv.floatCurrent, dtv.linearColorCurrent, dtv.rotatorCurrent, dtv.vectorCurrent, dtv.vector2DCurrent, alpha);
			}
			else
			{
				interf->OnTweenTick(this, actorTweening, componentTweening, widgetTweening, tweenName, alpha);
			}
		}
		else if (type == "loop")
		{
			interf->OnTweenLoop(this, actorTweening, componentTweening, widgetTweening, tweenName, numberOfLoopSections, loopType, playingBackward);
		}
		else if (type == "complete")
		{
			interf->OnTweenComplete(this, actorTweening, componentTweening, widgetTweening, tweenName, sweepResult, successfulTransform);
		}
		else
		{
			UiTween::Print("Target is not defined as iTweenEvent message-accepting type.", "error");
		}
	}
	else
	{
     */
		if (target && target->GetClass()->ImplementsInterface(UiTInterface::StaticClass()))
		{
			if (type == "start")
			{
				IiTInterface::Execute_OnTweenStart(target, this, actorTweening, componentTweening, widgetTweening, tweenName);
			}
			else if (type == "update")
			{
				//IiTInterface::Execute_OnTweenUpdate(target, this, actorTweening, componentTweening, widgetTweening, tweenName, dtv, alpha);

				if (IsEventDataType())
				{
					IiTInterface::Execute_OnTweenDataTick(target, this, tweenName, dtv.floatCurrent, dtv.linearColorCurrent, dtv.rotatorCurrent, dtv.vectorCurrent, dtv.vector2DCurrent, alpha);
				}
				else
				{
					IiTInterface::Execute_OnTweenTick(target, this, actorTweening, componentTweening, widgetTweening, tweenName, alpha);
				}
			}
			else if (type == "loop")
			{
				IiTInterface::Execute_OnTweenLoop(target, this, actorTweening, componentTweening, widgetTweening, tweenName, numberOfLoopSections, loopType, playingBackward);
			}
			else if (type == "complete")
			{
				IiTInterface::Execute_OnTweenComplete(target, this, actorTweening, componentTweening, widgetTweening, tweenName, sweepResult, successfulTransform);
			}
			else
			{
				UiTween::Print("Target is not defined as iTweenEvent message-accepting type.", "error");
			}
		}
		else
		{
			/*if (target)
			{
			UiTween::Print("Cannot send Interface message to" + target->GetFName().ToString(), "error");
			}
			else
			{
			UiTween::Print("Cannot send Interface message.", "error");
			}*/
		}
	//}
}

void AiTweenEvent::RunSlateInterface(STweenableWidget* target, FString type)
{
	STweenableWidget* interf = target;
	if (interf)
	{
		if (type == "start")
		{
			interf->OnTweenStart(this, actorTweening, componentTweening, widgetTweening, tweenName);
		}
		else if (type == "update")
		{
			//interf->OnTweenUpdate(this, actorTweening, componentTweening, widgetTweening, tweenName, dtv, alpha);

			if (IsEventDataType())
			{
				interf->OnTweenDataTick(this, tweenName, dtv.floatCurrent, dtv.linearColorCurrent, dtv.rotatorCurrent, dtv.vectorCurrent, dtv.vector2DCurrent, alpha);
			}
			else
			{
				interf->OnTweenTick(this, actorTweening, componentTweening, widgetTweening, tweenName, alpha);
			}
		}
		else if (type == "loop")
		{
			interf->OnTweenLoop(this, actorTweening, componentTweening, widgetTweening, tweenName, numberOfLoopSections, loopType, playingBackward);
		}
		else if (type == "complete")
		{
			interf->OnTweenComplete(this, actorTweening, componentTweening, widgetTweening, tweenName, sweepResult, successfulTransform);
		}
		else
		{
			UiTween::Print("Target is not defined as iTweenEvent message-accepting type.", "error");
		}
	}

	else
	{
		/*if (target)
		{
		UiTween::Print("Cannot send Interface message to" + target->GetFName().ToString(), "error");
		}
		else
		{
		UiTween::Print("Cannot send Interface message.", "error");
		}*/
	}
}

void AiTweenEvent::CheckLoopType()
{
	switch (loopType)
	{
	case once:
		EndPhase();
		break;
	case ELoopType::rewind:
		LoopSectionStuff();
		if (delayType == first)
		{
			LoopWithoutDelay();
		}
		else
		{
			LoopWithDelay();
		}
		break;
	case pingPong:
	{
					 playingBackward = !playingBackward;
					 LoopSectionStuff();
					 SwitchValues();
					 /*if (isLocal)
					 {
					 SpacializeValues();
					 }*/
					 if (delayType == first)
					 {
						 LoopWithoutDelay();
					 }
					 else if (delayType == firstLoop || delayType == loop)
					 {
						 LoopWithDelay();
					 }
					 else
					 {
						 if (playingBackward)
						 {
							 LoopWithoutDelay();
						 }
						 else
						 {
							 LoopWithDelay();
						 }
					 }
					 break;
	}
	default:
		EndPhase();
		break;
	}
}

void AiTweenEvent::CheckIfFirstDelay()
{
	if (delayType == first || delayType == firstLoop || delayType == firstLoopFull)
	{
		shouldDelay = true;
	}
}

void AiTweenEvent::InitEvent()
{
	if (aux && aux->performDebugOperations)
	{
		NameEventActor();
	}

	//Validate nodes
	CheckTargetValidity();

	//Check if delay is needed
	CheckIfFirstDelay();

	IgnorePauseTimeDilationSettings();

	SpacializeValues();

	if (interpolateToSpline && splineComponent != nullptr)
	{
		if (eventType == actorMoveToSplinePoint || eventType == compMoveToSplinePoint || eventType == actorRotateToSplinePoint || eventType == compRotateToSplinePoint)
		{
			ReconstructSpline();
		}
	}

	//Convert time to speed (if desired and applicable)
	SetTickTypeValue();

	SetTimerInterval(timerInterval);

	shouldTick = true;
}

void AiTweenEvent::ReconstructSpline()
{
	TArray<FVector> splinePoints;
	if (eventType == actorMoveToSplinePoint)
	{
		splinePoints.Add(actorTweening->GetActorLocation());
	}
	else if (eventType == compMoveToSplinePoint)
	{
		splinePoints.Add(componentTweening->GetComponentLocation());
	}
	else if (eventType == actorRotateToSplinePoint)
	{
		splinePoints.Add(actorTweening->GetActorLocation() + (actorTweening->GetActorForwardVector() * generatedPointDistance));
	}
	else if (eventType == compRotateToSplinePoint)
	{
		splinePoints.Add(componentTweening->GetComponentLocation() + (componentTweening->GetForwardVector() * generatedPointDistance));
	}
	for (int i = 0; i < splineComponent->GetNumSplinePoints(); i++)
	{
		splinePoints.Add(splineComponent->GetWorldLocationAtSplinePoint(i));
	}
	splineComponent->ClearSplinePoints();
	for (int i = 0; i < splinePoints.Num(); i++)
	{
		splineComponent->AddSplineWorldPoint(splinePoints[i]);
	}
}

void AiTweenEvent::NameEventActor()
{
	//this->SetActorLabel(*(tweenName.ToString() + " - " + FString::FromInt(tweenIndex) + " - " + GetEventTypeEnumAsFString(eventType)));
}

void AiTweenEvent::CheckTargetValidity()
{
	if (!orientationTarget)
	{
		orientationTarget = aux;

		if (orientation == orientToTarget)
		{
			orientation = orientToPath;
			UiTween::Print("No orientation target was specified. Orientation type has been defaulted to orientToPath.", "error");
		}
	}

	if (!onTweenStartTarget)
	{
		onTweenStartTarget = aux;
	}

	if (!onTweenUpdateTarget)
	{
		onTweenUpdateTarget = aux;
	}

	if (!onTweenLoopTarget)
	{
		onTweenLoopTarget = aux;
	}

	if (!onTweenCompleteTarget)
	{
		onTweenCompleteTarget = aux;
	}

	if (!customEaseTypeCurve && easeType == EEaseType::customCurve)
	{
		UiTween::Print("No Curve defined! You must specify a CurveFloat object that begins and a time of 0 and ends at a time of 1. Defaulting to Linear (No Easing).", "error");
	}
}

void AiTweenEvent::FirstSet()
{
	if (!firstSet)
	{
		if (eventType == actorMoveFromTo && actorTweening)
		{
			actorTweening->SetActorLocation(UiTween::ConstrainVector(vectorFrom, actorTweening->GetActorLocation(), vectorConstraints));
		}
		else if (eventType == actorRotateFromTo && actorTweening)
		{
			actorTweening->SetActorRotation(UiTween::ConstrainRotator(rotatorFrom, actorTweening->GetActorRotation(), rotatorConstraints));
		}
		else if (eventType == actorScaleFromTo && actorTweening)
		{
			actorTweening->SetActorScale3D(UiTween::ConstrainVector(vectorFrom, actorTweening->GetActorScale3D(), vectorConstraints));
		}
		else if (eventType == compMoveFromTo && componentTweening)
		{
			componentTweening->SetWorldLocation(UiTween::ConstrainVector(vectorFrom, componentTweening->GetComponentLocation(), vectorConstraints));
		}
		else if (eventType == compRotateFromTo && componentTweening)
		{
			componentTweening->SetWorldRotation(UiTween::ConstrainRotator(rotatorFrom, componentTweening->GetComponentRotation(), rotatorConstraints));
		}
		else if (eventType == compScaleFromTo && componentTweening)
		{
			componentTweening->SetWorldScale3D(UiTween::ConstrainVector(vectorFrom, componentTweening->GetComponentScale(), vectorConstraints));
		}
		else if (eventType == umgRTMoveFromTo && widgetTweening)
		{
			widgetTweening->SetRenderTranslation(UiTween::ConstrainVector2D(vector2DFrom, widgetTweening->RenderTransform.Translation, vector2DConstraints));
		}
		else if (eventType == umgRTRotateFromTo && widgetTweening)
		{
			widgetTweening->SetRenderAngle(floatFrom);
		}
		else if (eventType == umgRTScaleFromTo && widgetTweening)
		{
			widgetTweening->SetRenderScale(UiTween::ConstrainVector2D(vector2DFrom, widgetTweening->RenderTransform.Scale, vector2DConstraints));
		}
		else if (eventType == umgRTShearFromTo && widgetTweening)
		{
			widgetTweening->SetRenderShear(UiTween::ConstrainVector2D(vector2DFrom, widgetTweening->RenderTransform.Shear, vector2DConstraints));
		}
		else if (eventType == floatFromTo)
		{
			dtv.floatCurrent = floatFrom;
		}
		else if (eventType == vectorFromTo)
		{
			dtv.vectorCurrent = vectorFrom;
		}
		else if (eventType == vector2DFromTo)
		{
			dtv.vector2DCurrent = vector2DFrom;
		}
		else if (eventType == rotatorFromTo)
		{
			dtv.rotatorCurrent = rotatorFrom;
		}
		else if (eventType == linearColorFromTo)
		{
			dtv.linearColorCurrent = linearColorFrom;
		}
		else if (eventType == slateMoveFromTo)
		{
			slateTweening->SetTweenTranslation(UiTween::ConstrainVector2D(vector2DFrom, slateTweening->TweenTransform.Translation, vector2DConstraints));
		}
		else if (eventType == slateScaleFromTo)
		{
			slateTweening->SetTweenTranslation(UiTween::ConstrainVector2D(vector2DFrom, slateTweening->TweenTransform.Scale, vector2DConstraints));
		}
		else
		{
			if (tieToObjectValidity)
			{
				EndPhase();
			}
		}

		firstSet = true;
	}
}

void AiTweenEvent::InterpTween()
{
	if (eventType == actorMoveFromTo && actorTweening)
	{
		TickActorMoveFromTo();
	}
	else if (eventType == actorRotateFromTo && actorTweening)
	{
		TickActorRotateFromTo();
	}
	else if (eventType == actorScaleFromTo && actorTweening)
	{
		actorTweening->SetActorScale3D(UiTween::ConstrainVector(FMath::Lerp<FVector>(vectorFrom, vectorTo, GetAlphaFromEquation(alpha)), actorTweening->GetActorScale3D(), vectorConstraints));
	}
	else if (eventType == compMoveFromTo && componentTweening)
	{
		TickComponentMoveFromTo();
	}
	else if (eventType == compRotateFromTo && componentTweening)
	{
		TickComponentRotateFromTo();
	}
	else if (eventType == compScaleFromTo && componentTweening)
	{
		componentTweening->SetWorldScale3D(UiTween::ConstrainVector(FMath::Lerp<FVector>(vectorFrom, vectorTo, GetAlphaFromEquation(alpha)), componentTweening->GetComponentScale(), vectorConstraints));
	}
	else if (eventType == umgRTMoveFromTo && widgetTweening)
	{
		TickUMGRTMoveFromTo();
	}
	else if (eventType == umgRTRotateFromTo && widgetTweening)
	{
		widgetTweening->SetRenderAngle(FMath::Lerp<float>(floatFrom, floatTo, GetAlphaFromEquation(alpha)));
	}
	else if (eventType == umgRTScaleFromTo && widgetTweening)
	{
		FVector v = FMath::Lerp<FVector>(FVector(vector2DFrom.X, vector2DFrom.Y, 0), FVector(vector2DTo.X, vector2DTo.Y, 0), GetAlphaFromEquation(alpha));
		widgetTweening->SetRenderScale(UiTween::ConstrainVector2D(FVector2D(v.X, v.Y), widgetTweening->RenderTransform.Scale, vector2DConstraints));
	}
	else if (eventType == umgRTShearFromTo && widgetTweening)
	{
		FVector v = FMath::Lerp<FVector>(FVector(vector2DFrom.X, vector2DFrom.Y, 0), FVector(vector2DTo.X, vector2DTo.Y, 0), GetAlphaFromEquation(alpha));
		widgetTweening->SetRenderShear(UiTween::ConstrainVector2D(FVector2D(v.X, v.Y), widgetTweening->RenderTransform.Shear, vector2DConstraints));
	}
	else if (eventType == floatFromTo)
	{
		dtv.floatCurrent = FMath::Lerp<float>(floatFrom, floatTo, GetAlphaFromEquation(alpha));
	}
	else if (eventType == vectorFromTo)
	{
		dtv.vectorCurrent = UiTween::ConstrainVector(FMath::Lerp<FVector>(vectorFrom, vectorTo, GetAlphaFromEquation(alpha)), dtv.vectorCurrent, vectorConstraints);
	}
	else if (eventType == vector2DFromTo)
	{
		FVector v = FMath::Lerp<FVector>(FVector(vector2DFrom.X, vector2DFrom.Y, 0), FVector(vector2DTo.X, vector2DTo.Y, 0), GetAlphaFromEquation(alpha));
		dtv.vector2DCurrent = UiTween::ConstrainVector2D(FVector2D(v.X, v.Y), dtv.vector2DCurrent, vector2DConstraints);
	}
	else if (eventType == rotatorFromTo)
	{
		TickRotatorFromTo();
	}
	else if (eventType == linearColorFromTo)
	{
		dtv.linearColorCurrent = FMath::Lerp<FLinearColor>(linearColorFrom, linearColorTo, GetAlphaFromEquation(alpha));
	}
	else if (eventType == actorMoveToSplinePoint && actorTweening)
	{
		TickActorMoveToSplinePoint();
	}
	else if (eventType == compMoveToSplinePoint && componentTweening)
	{
		TickComponentMoveToSplinePoint();
	}
	else if (eventType == actorRotateToSplinePoint && actorTweening)
	{
		TickActorRotateToSplinePoint();
	}
	else if (eventType == compRotateToSplinePoint && componentTweening)
	{
		TickComponentRotateToSplinePoint();
	}
	else if (eventType == slateMoveFromTo)
	{
		TickSlateMoveFromTo();
	}
	else if (eventType == slateScaleFromTo)
	{
		FVector v = FMath::Lerp<FVector>(FVector(vector2DFrom.X, vector2DFrom.Y, 0), FVector(vector2DTo.X, vector2DTo.Y, 0), GetAlphaFromEquation(alpha));
		slateTweening->SetTweenScale(UiTween::ConstrainVector2D(FVector2D(v.X, v.Y), slateTweening->TweenTransform.Scale, vector2DConstraints));
	}
	else
	{
		if (tieToObjectValidity)
		{
			EndPhase();
		}
	}
}

void AiTweenEvent::LastSet()
{
	if (enforceValueTo)
	{
		if (eventType == actorMoveFromTo && actorTweening)
		{
			actorTweening->SetActorLocation(UiTween::ConstrainVector(vectorTo, actorTweening->GetActorLocation(), vectorConstraints), sweep);
		}
		else if (eventType == actorRotateFromTo && actorTweening)
		{
			actorTweening->SetActorRotation(UiTween::ConstrainRotator(rotatorTo, actorTweening->GetActorRotation(), rotatorConstraints));
		}
		else if (eventType == actorScaleFromTo && actorTweening)
		{
			actorTweening->SetActorScale3D(UiTween::ConstrainVector(vectorTo, actorTweening->GetActorScale3D(), vectorConstraints));
		}
		else if (eventType == compMoveFromTo && componentTweening)
		{
			componentTweening->SetWorldLocation(UiTween::ConstrainVector(vectorTo, componentTweening->GetComponentLocation(), vectorConstraints));
		}
		else if (eventType == compRotateFromTo && componentTweening)
		{
			componentTweening->SetWorldRotation(UiTween::ConstrainRotator(rotatorTo, componentTweening->GetComponentRotation(), rotatorConstraints));
		}
		else if (eventType == compScaleFromTo && componentTweening)
		{
			componentTweening->SetWorldScale3D(UiTween::ConstrainVector(vectorTo, componentTweening->GetComponentScale(), vectorConstraints));
		}
		else if (eventType == umgRTMoveFromTo && widgetTweening)
		{
			widgetTweening->SetRenderTranslation(UiTween::ConstrainVector2D(vector2DTo, widgetTweening->RenderTransform.Translation, vector2DConstraints));
		}
		else if (eventType == umgRTRotateFromTo && widgetTweening)
		{
			widgetTweening->SetRenderAngle(floatTo);
		}
		else if (eventType == umgRTScaleFromTo && widgetTweening)
		{
			widgetTweening->SetRenderScale(UiTween::ConstrainVector2D(vector2DTo, widgetTweening->RenderTransform.Scale, vector2DConstraints));
		}
		else if (eventType == umgRTShearFromTo && widgetTweening)
		{
			widgetTweening->SetRenderShear(UiTween::ConstrainVector2D(vector2DTo, widgetTweening->RenderTransform.Shear, vector2DConstraints));
		}
		else if (eventType == floatFromTo)
		{
			dtv.floatCurrent = floatTo;
		}
		else if (eventType == vectorFromTo)
		{
			dtv.vectorCurrent = vectorTo;
		}
		else if (eventType == vector2DFromTo)
		{
			dtv.vector2DCurrent = vector2DTo;
		}
		else if (eventType == rotatorFromTo)
		{
			dtv.rotatorCurrent = rotatorTo;
		}
		else if (eventType == linearColorFromTo)
		{
			dtv.linearColorCurrent = linearColorTo;
		}
		else if (eventType == actorMoveToSplinePoint && actorTweening)
		{
			EndActorMoveToSplinePoint();
		}
		else if (eventType == compMoveToSplinePoint && componentTweening)
		{
			EndComponentMoveToSplinePoint();
		}
		else if (eventType == actorRotateToSplinePoint && actorTweening)
		{
			EndActorRotateToSplinePoint();
		}
		else if (eventType == compRotateToSplinePoint && componentTweening)
		{
			EndComponentRotateToSplinePoint();
		}
		else if (eventType == slateMoveFromTo)
		{
			slateTweening->SetTweenTranslation(UiTween::ConstrainVector2D(vector2DTo, slateTweening->TweenTransform.Translation, vector2DConstraints));
		}
		else if (eventType == slateScaleFromTo)
		{
			slateTweening->SetTweenScale(UiTween::ConstrainVector2D(vector2DTo, slateTweening->TweenTransform.Scale, vector2DConstraints));
		}
		else
		{
			if (tieToObjectValidity)
			{
				EndPhase();
			}
		}
	}
}

void AiTweenEvent::ExecuteTween()
{
	if (shouldTick)
	{
		if (shouldTween)
		{
			alpha = FMath::Clamp<float>((alpha + (deltaSeconds / tickTypeValue)), 0.f, 1.f);

			InterpTween();

			//OnTweenTick Interface Message
			RunInterface(onTweenUpdateTarget, "update");
			RunSlateInterface(onTweenTickSlateTarget, "update");

			if (alpha >= 1.0f)
			{
				if (easeType != EEaseType::punch)
				{
					LastSet();
				}

				CheckLoopType();
			}
		}
		else
		{
			if (shouldDelay)
			{
				DelayTween();
			}
			else
			{
				//OnTweenStart Interface Message
				RunInterface(onTweenStartTarget, "start");
				RunSlateInterface(onTweenStartSlateTarget, "start");

				FirstSet();

				shouldTween = true;
			}
		}
	}
}

void AiTweenEvent::OrientationSwitch()
{
	switch (orientation)
	{
	case noOrientationChange:
		break;
	case orientToTarget:
		if (orientationTarget)
		{
			OrientToTargetImplementation();
			break;
		}
		else
		{
			OrientToPathImplementation();
			break;
		}
	case orientToPath:
		OrientToPathImplementation();
		break;
	default:
		break;
	}
}

void AiTweenEvent::OrientToTargetImplementation()
{
	if (eventType == actorMoveFromTo || eventType == actorMoveToSplinePoint)
	{
		if (orientationTarget->IsA(AActor::StaticClass()))
		{
			UiTween::ActorRotateUpdate(actorTweening, FRotationMatrix::MakeFromX(((AActor*)orientationTarget)->GetActorLocation() - actorTweening->GetActorLocation()).Rotator(), deltaSeconds, orientationSpeed, false, rotatorConstraints);
		}
		else if (orientationTarget->IsA(USceneComponent::StaticClass()))
		{
			UiTween::ActorRotateUpdate(actorTweening, FRotationMatrix::MakeFromX(((USceneComponent*)orientationTarget)->GetComponentLocation() - actorTweening->GetActorLocation()).Rotator(), deltaSeconds, orientationSpeed, false, rotatorConstraints);
		}
		else if (orientationTarget->IsA(UWidget::StaticClass()))
		{
			//Feature is still to come. Waiting on getting the ability to read widget position in screen space
		}

	}
	else if (eventType == compMoveFromTo || eventType == compMoveToSplinePoint)
	{
		if (orientationTarget->IsA(AActor::StaticClass()))
		{
			UiTween::ComponentRotateUpdate(componentTweening, FRotationMatrix::MakeFromX(((AActor*)orientationTarget)->GetActorLocation() - componentTweening->GetComponentLocation()).Rotator(), deltaSeconds, orientationSpeed, false, rotatorConstraints);
		}
		else if (orientationTarget->IsA(USceneComponent::StaticClass()))
		{
			UiTween::ComponentRotateUpdate(componentTweening, FRotationMatrix::MakeFromX(((USceneComponent*)orientationTarget)->GetComponentLocation() - componentTweening->GetComponentLocation()).Rotator(), deltaSeconds, orientationSpeed, false, rotatorConstraints);
		}
		else if (orientationTarget->IsA(UWidget::StaticClass()))
		{
			//Feature is still to come. Waiting on getting the ability to read widget position in screen space
		}
	}
	else if (eventType == umgRTMoveFromTo)
	{
		//Feature is still to come. Waiting on getting the ability to read widget position in screen space
		/*FVector2D loc;
		UiTween::GetWorldLocal()->GetFirstPlayerController()->ProjectWorldLocationToScreen(((AActor*)orientationTarget)->GetActorLocation(), loc);
		UiTween::UMGRTRotateUpdate(widgetTweening, loc, orientationSpeed, deltaSeconds);*/
	}
}

void AiTweenEvent::OrientToPathImplementation()
{
	if (eventType == actorMoveFromTo)
	{
		actorTweening->SetActorRotation(UiTween::ConstrainRotator(FMath::RInterpTo(actorTweening->GetActorRotation(), FRotationMatrix::MakeFromX((UiTween::ConstrainVector(vectorTo, actorTweening->GetActorLocation(), vectorConstraints)) - vectorFrom).Rotator(), deltaSeconds, orientationSpeed), actorTweening->GetActorRotation(), rotatorConstraints));
	}
	else if (eventType == compMoveFromTo)
	{
		componentTweening->SetWorldRotation(UiTween::ConstrainRotator(FMath::RInterpTo(componentTweening->GetComponentRotation(), FRotationMatrix::MakeFromX((UiTween::ConstrainVector(vectorTo, componentTweening->GetComponentLocation(), vectorConstraints)) - vectorFrom).Rotator(), deltaSeconds, orientationSpeed), componentTweening->GetComponentRotation(), rotatorConstraints));
	}
	else if (eventType == actorMoveToSplinePoint)
	{
		if (playingBackward && switchPathOrientationDirection)
		{
			actorTweening->SetActorRotation(UiTween::ConstrainRotator(FMath::RInterpTo(actorTweening->GetActorRotation(), (splineComponent->GetWorldRotationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration).Quaternion().Inverse().Rotator()), deltaSeconds, orientationSpeed), actorTweening->GetActorRotation(), rotatorConstraints));
		}
		else
		{
			actorTweening->SetActorRotation(UiTween::ConstrainRotator(FMath::RInterpTo(actorTweening->GetActorRotation(), splineComponent->GetWorldRotationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration), deltaSeconds, orientationSpeed), actorTweening->GetActorRotation(), rotatorConstraints));
		}
	}
	else if (eventType == compMoveToSplinePoint)
	{
		if (playingBackward && switchPathOrientationDirection)
		{
			componentTweening->SetWorldRotation(UiTween::ConstrainRotator(FMath::RInterpTo(componentTweening->GetComponentRotation(), (splineComponent->GetWorldRotationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration).Quaternion().Inverse().Rotator()), deltaSeconds, orientationSpeed), componentTweening->GetComponentRotation(), rotatorConstraints));
		}
		else
		{
			componentTweening->SetWorldRotation(UiTween::ConstrainRotator(FMath::RInterpTo(componentTweening->GetComponentRotation(), splineComponent->GetWorldRotationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration), deltaSeconds, orientationSpeed), componentTweening->GetComponentRotation(), rotatorConstraints));
		}
	}
	else if (eventType == umgRTMoveFromTo)
	{
		//Feature is still to come. Waiting on getting the ability to read widget position in screen space
		/*FVector2D loc;
		UiTween::GetWorldLocal()->GetFirstPlayerController()->ProjectWorldLocationToScreen(((AActor*)orientationTarget)->GetActorLocation(), loc);
		UiTween::UMGRTRotateUpdate(widgetTweening, loc, orientationSpeed, deltaSeconds);*/
	}
}

void AiTweenEvent::TickActorMoveFromTo()
{
	successfulTransform = actorTweening->SetActorLocation(UiTween::ConstrainVector(FMath::Lerp<FVector>(vectorFrom, vectorTo, GetAlphaFromEquation(alpha)), actorTweening->GetActorLocation(), vectorConstraints), sweep, &sweepResult);
	if (sweep)
	{
		if (sweepResult.bBlockingHit)
		{
			EndPhase();
		}
	}

	OrientationSwitch();
}

void AiTweenEvent::TickActorRotateFromTo()
{
	if (shortestPath)
	{
		FQuat rot = FQuat::Slerp(rotatorFrom.Quaternion(), rotatorTo.Quaternion(), GetAlphaFromEquation(alpha));

		rot.Normalize();

		successfulTransform = actorTweening->SetActorRotation(UiTween::ConstrainRotator(rot.Rotator(), actorTweening->GetActorRotation(), rotatorConstraints));
	}
	else
	{
		successfulTransform = actorTweening->SetActorRotation(UiTween::ConstrainRotator(FMath::LerpStable<FRotator>(rotatorFrom, rotatorTo, GetAlphaFromEquation(alpha)), actorTweening->GetActorRotation(), rotatorConstraints));
	}
}

void AiTweenEvent::TickComponentMoveFromTo()
{
	componentTweening->SetWorldLocation(UiTween::ConstrainVector(FMath::Lerp<FVector>(vectorFrom, vectorTo, GetAlphaFromEquation(alpha)), componentTweening->GetComponentLocation(), vectorConstraints), sweep, &sweepResult);
	if (sweep)
	{
		if (sweepResult.bBlockingHit)
		{
			successfulTransform = false;
			EndPhase();
		}
	}

	OrientationSwitch();
}

void AiTweenEvent::TickComponentRotateFromTo()
{
	if (shortestPath)
	{
		FQuat rot = FQuat::Slerp(rotatorFrom.Quaternion(), rotatorTo.Quaternion(), GetAlphaFromEquation(alpha));

		rot.Normalize();

		componentTweening->SetWorldRotation(UiTween::ConstrainRotator(rot.Rotator(), componentTweening->GetComponentRotation(), rotatorConstraints));
	}
	else
	{
		componentTweening->SetWorldRotation(UiTween::ConstrainRotator(FMath::LerpStable<FRotator>(rotatorFrom, rotatorTo, GetAlphaFromEquation(alpha)), componentTweening->GetComponentRotation(), rotatorConstraints));
	}
}

void AiTweenEvent::TickUMGRTMoveFromTo()
{
	FVector v = FMath::Lerp<FVector>(FVector(vector2DFrom.X, vector2DFrom.Y, 0), FVector(vector2DTo.X, vector2DTo.Y, 0), GetAlphaFromEquation(alpha));
	widgetTweening->SetRenderTranslation(UiTween::ConstrainVector2D(FVector2D(v.X, v.Y), widgetTweening->RenderTransform.Translation, vector2DConstraints));

	OrientationSwitch();
}

void AiTweenEvent::TickSlateMoveFromTo()
{
	FVector v = FMath::Lerp<FVector>(FVector(vector2DFrom.X, vector2DFrom.Y, 0), FVector(vector2DTo.X, vector2DTo.Y, 0), GetAlphaFromEquation(alpha));
	slateTweening->SetTweenTranslation(UiTween::ConstrainVector2D(FVector2D(v.X, v.Y), slateTweening->TweenTransform.Translation, vector2DConstraints));

	OrientationSwitch();
}

void AiTweenEvent::TickRotatorFromTo()
{
	dtv.rotatorCurrent = rotatorFrom;
	if (shortestPath)
	{
		FQuat rot = FQuat::Slerp(rotatorFrom.Quaternion(), rotatorTo.Quaternion(), GetAlphaFromEquation(alpha));

		rot.Normalize();

		dtv.rotatorCurrent = UiTween::ConstrainRotator(rot.Rotator(), dtv.rotatorCurrent, rotatorConstraints);
	}
	else
	{
		dtv.rotatorCurrent = UiTween::ConstrainRotator(FMath::LerpStable<FRotator>(rotatorFrom, rotatorTo, GetAlphaFromEquation(alpha)), dtv.rotatorCurrent, rotatorConstraints);
	}
}

void AiTweenEvent::TickActorMoveToSplinePoint()
{
	if (playingBackward)
	{
		successfulTransform = actorTweening->SetActorLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(FMath::Abs((GetAlphaFromEquation(alpha) * splineComponent->Duration) - splineComponent->Duration)), actorTweening->GetActorLocation(), vectorConstraints), sweep, &sweepResult);
	}
	else
	{
		successfulTransform = actorTweening->SetActorLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration), actorTweening->GetActorLocation(), vectorConstraints), sweep, &sweepResult);
	}

	if (sweep)
	{
		if (sweepResult.bBlockingHit)
		{
			EndPhase();
		}
	}

	OrientationSwitch();
}

void AiTweenEvent::TickComponentMoveToSplinePoint()
{
	if (playingBackward)
	{
		componentTweening->SetWorldLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(FMath::Abs((GetAlphaFromEquation(alpha) * splineComponent->Duration) - splineComponent->Duration)), componentTweening->GetComponentLocation(), vectorConstraints), sweep, &sweepResult);
	}
	else
	{
		componentTweening->SetWorldLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration), componentTweening->GetComponentLocation(), vectorConstraints), sweep, &sweepResult);
	}

	if (sweep)
	{
		if (sweepResult.bBlockingHit)
		{
			successfulTransform = false;
			EndPhase();
		}
	}

	OrientationSwitch();
}

void AiTweenEvent::TickActorRotateToSplinePoint()
{
	if (playingBackward)
	{
		actorTweening->SetActorRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(FMath::Abs((GetAlphaFromEquation(alpha) * splineComponent->Duration) - splineComponent->Duration))) - actorTweening->GetActorLocation()).Rotator(), actorTweening->GetActorRotation(), rotatorConstraints));
	}
	else
	{
		actorTweening->SetActorRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration)) - actorTweening->GetActorLocation()).Rotator(), actorTweening->GetActorRotation(), rotatorConstraints));
	}
}

void AiTweenEvent::TickComponentRotateToSplinePoint()
{
	if (playingBackward)
	{
		componentTweening->SetWorldRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(FMath::Abs((GetAlphaFromEquation(alpha) * splineComponent->Duration) - splineComponent->Duration))) - componentTweening->GetComponentLocation()).Rotator(), componentTweening->GetComponentRotation(), rotatorConstraints));
	}
	else
	{
		componentTweening->SetWorldRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration)) - componentTweening->GetComponentLocation()).Rotator(), componentTweening->GetComponentRotation(), rotatorConstraints));
	}
}

void AiTweenEvent::EndActorMoveToSplinePoint()
{
	if (playingBackward)
	{
		actorTweening->SetActorLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(FMath::Abs((1.f * splineComponent->Duration) - splineComponent->Duration)), actorTweening->GetActorLocation(), vectorConstraints), sweep, &sweepResult);
	}
	else
	{
		actorTweening->SetActorLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(1.f * splineComponent->Duration), actorTweening->GetActorLocation(), vectorConstraints), sweep, &sweepResult);
	}
}

void AiTweenEvent::EndComponentMoveToSplinePoint()
{
	if (playingBackward)
	{
		componentTweening->SetWorldLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(FMath::Abs((1.f * splineComponent->Duration) - splineComponent->Duration)), componentTweening->GetComponentLocation(), vectorConstraints), sweep, &sweepResult);
	}
	else
	{
		componentTweening->SetWorldLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(1.f * splineComponent->Duration), componentTweening->GetComponentLocation(), vectorConstraints), sweep, &sweepResult);
	}
}

void AiTweenEvent::EndActorRotateToSplinePoint()
{
	if (playingBackward)
	{
		actorTweening->SetActorRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(FMath::Abs((1.f * splineComponent->Duration) - splineComponent->Duration))) - actorTweening->GetActorLocation()).Rotator(), actorTweening->GetActorRotation(), rotatorConstraints));
	}
	else
	{
		actorTweening->SetActorRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(1.f * splineComponent->Duration)) - actorTweening->GetActorLocation()).Rotator(), actorTweening->GetActorRotation(), rotatorConstraints));
	}
}

void AiTweenEvent::EndComponentRotateToSplinePoint()
{
	if (playingBackward)
	{
		componentTweening->SetWorldRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(FMath::Abs((1.f * splineComponent->Duration) - splineComponent->Duration))) - componentTweening->GetComponentLocation()).Rotator(), componentTweening->GetComponentRotation(), rotatorConstraints));
	}
	else
	{
		componentTweening->SetWorldRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(1.f * splineComponent->Duration)) - componentTweening->GetComponentLocation()).Rotator(), componentTweening->GetComponentRotation(), rotatorConstraints));
	}
}
