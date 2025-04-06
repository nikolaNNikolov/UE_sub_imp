//Sub_Imp Plugin
#include "SSubImpWindow.h"

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "PropertyCustomizationHelpers.h"
#include "Interfaces/IMainFrameModule.h"
#include "Subsystems/EditorAssetSubsystem.h"
#include "ThumbnailRendering/ThumbnailManager.h"


class IMainFrameModule;

SSubImpWindow::SSubImpWindow()
{
}

void SSubImpWindow::Construct(const FArguments& InArgs)
{
	//todo: all logging feedback
	ResetSubImp();
	ParentWindow = InArgs._ParentWindow;
	
	ChildSlot
	[
		SNew(SVerticalBox)

		
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			SNew(STextBlock)
			.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
			.Justification(ETextJustify::Left)
			.Text(FText::FromString("Select .srt file: "))
		]

		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		[
			SNew(SBox)
			.WidthOverride(350.0f)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				[
					SNew(SEditableTextBox)
					.IsReadOnly(true)
					.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
					.Text(this, &SSubImpWindow::GetLoadedFileStringAsText)
				]
				+SVerticalBox::Slot()
				.HAlign(HAlign_Left)
				[
					SNew(SBox)
					.WidthOverride(75.0f)
					.VAlign(VAlign_Fill)
					.Padding(0.0f, 5.0f, 0.0f, 0.0f)
					[
						SNew(SButton)
						.VAlign(VAlign_Fill)
						.OnClicked(this, &SSubImpWindow::OpenSRTFilePickerWindow)
						[
							SNew(STextBlock)
							.Font(FAppStyle::Get().GetFontStyle("SmallFont"))
							.Justification(ETextJustify::Left)
							.Text(FText::FromString("Browse"))
						]
					]
				]
			]
			
		]

		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			SNew(STextBlock)
			.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
			.Justification(ETextJustify::Left)
			.Text(FText::FromString("Select Sound Wave file: "))
		]
		
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		[
			SNew(SBox)
			.WidthOverride(350.0f)
			[
				SNew(SObjectPropertyEntryBox)
				.ThumbnailPool(UThumbnailManager::Get().GetSharedThumbnailPool())
				.DisplayThumbnail(true)
				.DisplayBrowse(true)
				.EnableContentPicker(true)
				.AllowedClass(USoundWave::StaticClass())
				.ObjectPath(this, &SSubImpWindow::GetSelectedSoundWavePath)
				.OnObjectChanged(this, &SSubImpWindow::OnSoundWaveSelected)
			]
		]

		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			SNew(SButton)
			.Text(FText::FromString("GENERATE SUBTITLES"))
			.IsEnabled(this, &SSubImpWindow::GetIsGenerateButtonEnabled)
			.OnClicked(this, &SSubImpWindow::DoTheSubImp)
		]
	];
}

FReply SSubImpWindow::OpenSRTFilePickerWindow()
{
	GeneratedSubtitleInfo.Empty();
	TArray<FString> ReadTextFiles;

	if(ParentWindow == nullptr)
		return FReply::Handled();
	
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	bool FoundFile = false;
	if(DesktopPlatform)
	{
		FoundFile = DesktopPlatform->OpenFileDialog(ParentWindow->GetNativeWindow()->GetOSWindowHandle(),
			OPEN_FILE_DIALOG_TITLE,
			FPaths::ProjectContentDir(),
			OPEN_FILE_DIALOG_DEFAULT_FILE,
			OPEN_FILE_DIALOG_FILE_TYPES,
			OPEN_FILE_DIALOG_FLAGS,
			ReadTextFiles);
	}

	//todo: move to func - GenerateSubtitleCueArrayFromString
	// file read successfully, generate the subtitles
	if (FoundFile && !ReadTextFiles.IsEmpty())
	{
		const FString FilePath = ReadTextFiles[0];
		LoadedFileString = FilePath;

		TArray<FString> LoadedText;
		int SubtitleIndex = -1;
		float CachedSubtitleEndTime = 0.0f;
		FSubtitleCue SubtitleCue = FSubtitleCue();
		
		FFileHelper::LoadFileToStringArray(LoadedText, *FilePath);

		for (int i = 0; i <= LoadedText.Num() - 1; i++)
		{
			FString Line = LoadedText[i];
			if(!Line.IsEmpty())
			{
				if(Line.IsNumeric())
				{
					// push generated subtitle cue into the wave file
					const int NewSubtitleIndex = FCString::Atoi(*Line);
					if (NewSubtitleIndex > SubtitleIndex)
					{
						SubtitleIndex = NewSubtitleIndex;

						// Subtitle Cue is valid, push
						if(!SubtitleCue.Text.IsEmpty() && SubtitleCue.Time >= 0.0f)
						{
							GeneratedSubtitleInfo.Add(SubtitleCue);
							SubtitleCue = FSubtitleCue();	
						}
					}
					continue;
				}
				
				if(Line.Contains(SUBTITLE_TIME_DELIMITER))
				{
					FString LeftString, RightString;
					Line.Split(SUBTITLE_TIME_DELIMITER, &LeftString, &RightString);

					const float CurrentSubtitleStartTime = GetTotalSecondsFromTimespanString(LeftString);

					if (CurrentSubtitleStartTime - CachedSubtitleEndTime >=EndSubTagTimeout)
					{
						FSubtitleCue EmptySubtitleCue = FSubtitleCue();
						EmptySubtitleCue.Text = EndSubTag;
						EmptySubtitleCue.Time = CachedSubtitleEndTime;
						
						GeneratedSubtitleInfo.Add(EmptySubtitleCue);
					}
						
					SubtitleCue.Time = CurrentSubtitleStartTime;
					CachedSubtitleEndTime = GetTotalSecondsFromTimespanString(RightString);
					
					continue;
				}

				//todo: handle case with loc string
				if(SubtitleCue.Text.IsEmpty())
				{
					SubtitleCue.Text = FText::FromString(Line);
				}
				else
				{
					FString TextFormat = "{0}" + FString(LINE_TERMINATOR) + "{1}";
					const FText CurrentSubtitleCue = SubtitleCue.Text;
					const FText NewSubtitleCue = FText::FromString(Line);
					SubtitleCue.Text = FText::Format(FText::FromString(TextFormat), CurrentSubtitleCue, NewSubtitleCue);
				}
			}
		}
	}
	
	return FReply::Handled();
}

FReply SSubImpWindow::DoTheSubImp()
{
	if(SelectedSoundWave == nullptr
	|| GeneratedSubtitleInfo.IsEmpty()
	|| !GEditor)
		return FReply::Handled();
	
	SelectedSoundWave->Subtitles = GeneratedSubtitleInfo;
	
	//Save asset
	UEditorAssetSubsystem* EditorAssetSubsystem = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>();
	if(EditorAssetSubsystem->SaveAsset(SelectedSoundWavePath, true))
	{
		EditorAssetSubsystem->CheckoutAsset(SelectedSoundWavePath);
	}

	ResetSubImp();
	
	return FReply::Handled();
}

void SSubImpWindow::OnSoundWaveSelected(const FAssetData& InSoundWave)
{
	if(InSoundWave.IsValid())
	{
		USoundWave* SoundWaveAsset = static_cast<USoundWave*>(InSoundWave.GetAsset());

		if(SoundWaveAsset == nullptr)
		{
			return;
		}

		SelectedSoundWave = SoundWaveAsset;
		SelectedSoundWavePath = InSoundWave.GetObjectPathString();
	}
}

bool SSubImpWindow::GetIsGenerateButtonEnabled() const
{
	return (SelectedSoundWave != nullptr) && !GeneratedSubtitleInfo.IsEmpty();
}

FText SSubImpWindow::GetLoadedFileStringAsText() const
{
	return FText::FromString(LoadedFileString);
}

FString SSubImpWindow::GetSelectedSoundWavePath() const
{
	return SelectedSoundWavePath;
}

void SSubImpWindow::ResetSubImp()
{
	LoadedFileString = "Choose File...";
	GeneratedSubtitleInfo.Empty();
	SelectedSoundWave = nullptr;
	SelectedSoundWavePath = FString();
}

float SSubImpWindow::GetTotalSecondsFromTimespanString(const FString& TimespanString) const
{
	TArray<FString> InternalTimespanStrings;
	int TimespanHours = 0;
	int TimespanMinutes = 0;
	int TimespanSeconds = 0;

	TimespanString.ParseIntoArray(InternalTimespanStrings, TIMESPAN_DELIMITER);

	for (int i = 0; i <= InternalTimespanStrings.Num() - 1; i++)
	{
		FString CurrentIndex = InternalTimespanStrings[i];
		switch(i)
		{
		case 0:
			{
				TimespanHours = FCString::Atoi(*CurrentIndex);
				break;
			}
		case 1:
			{
				TimespanMinutes = FCString::Atoi(*CurrentIndex);
				break;
			}
		case 2:
			{
				FString LeftSecondsString, RightSecondsString;
				CurrentIndex.Split(TEXT(","), &LeftSecondsString, &RightSecondsString);
				TimespanSeconds = FCString::Atoi(*LeftSecondsString);
				break;
			}
		default:
			break;
		}
	}
	
	return FTimespan(TimespanHours, TimespanMinutes, TimespanSeconds).GetTotalSeconds();
}

///////////////////////////////////////////////////////////////////////////

#define WINDOW_SIZE FVector2D(350.0f, 300.0f)
#define MIN_MOUSE_OFFSET 15.0f

void OpenSubImpWindow(TSharedPtr<SWindow> ParentWindow)
{
	CloseSubImpWindow();

	const FVector2D CursorPosition = FSlateApplication::Get().GetCursorPos();
	const FSlateRect CursorAnchor (CursorPosition.X - WINDOW_SIZE.X/2, CursorPosition.Y + MIN_MOUSE_OFFSET,
		CursorPosition.X - WINDOW_SIZE.X/2, CursorPosition.Y + MIN_MOUSE_OFFSET);
	const FVector2D SpawnLocation = FSlateApplication::Get().CalculatePopupWindowPosition(CursorAnchor, WINDOW_SIZE);

	TSharedPtr<SWindow> Window = SNew(SWindow)
		.ScreenPosition(SpawnLocation)
		.AutoCenter(EAutoCenter::None)
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		.SizingRule(ESizingRule::FixedSize)
		.ClientSize(WINDOW_SIZE)
		.HasCloseButton(true)
		.Title(FText::FromString("sub-imp"))
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
			.Padding(FMargin(8.0f))
			[
				SNew(SSubImpWindow)
				.ParentWindow(ParentWindow)
			]
		];

	if (ParentWindow.IsValid())
	{
		Window = FSlateApplication::Get().AddWindowAsNativeChild(Window.ToSharedRef(), ParentWindow.ToSharedRef());
	}
	else
	{
		Window = FSlateApplication::Get().AddWindow(Window.ToSharedRef());
	}
	
	CurrentSubImpWindow = Window;
	
}

void CloseSubImpWindow()
{
	if(CurrentSubImpWindow.IsValid())
	{
		CurrentSubImpWindow.Pin()->RequestDestroyWindow();
		CurrentSubImpWindow.Reset();
	}
}

///////////////////////////////////////////////////////////////////////////

