// Copyright Epic Games, Inc. All Rights Reserved.

#include "SubImpStyle.h"
#include "SubImp.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FSubImpStyle::StyleInstance = nullptr;

void FSubImpStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FSubImpStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FSubImpStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("SubImpStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FSubImpStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("SubImpStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("SubImp")->GetBaseDir() / TEXT("Resources"));

	Style->Set("sub-imp.PluginAction", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));
	Style->Set("sub-imp.PluginAction.Small", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon16x16));
	return Style;
}

void FSubImpStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FSubImpStyle::Get()
{
	return *StyleInstance;
}
