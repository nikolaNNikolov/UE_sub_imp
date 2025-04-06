//Sub_Imp Plugin
#include "SSubImpWindow.h"

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "PropertyCustomizationHelpers.h"
#include "Components/SlateWrapperTypes.h"
#include "Interfaces/IMainFrameModule.h"
#include "Internationalization/StringTable.h"
#include "Subsystems/EditorAssetSubsystem.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SWidgetSwitcher.h"


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
			.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
			.Justification(ETextJustify::Left)
			.Text(FText::FromString("Select .srt file: "))
			.ToolTipText(FText::FromString("The .srt file from which to generate the Sound Wave subtitles."))
		]

		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			SNew(SBox)
			.WidthOverride(350.0f)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SEditableTextBox)
					.IsReadOnly(true)
					.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
					.Text(this, &SSubImpWindow::GetLoadedFileStringAsText)
					.ToolTipText(this, &SSubImpWindow::GetLoadedFileTooltipText)
				]
				+SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.Padding(10.0f, 0.0f, 0.0f, 0.0f)
				.AutoWidth()
				[
					SNew(SBox)
					.VAlign(VAlign_Center)
					.ToolTipText(FText::FromString("Browse"))
					[
						SNew(SButton)
						.VAlign(VAlign_Fill)
						.OnClicked(this, &SSubImpWindow::OpenSRTFilePickerWindow)
						.ContentPadding(FMargin(0.0f, 4.0f))
						.Content()
						[
							SNew(SImage)
							.Image(FAppStyle::Get().GetBrush("Icons.Search"))
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
			.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
			.Justification(ETextJustify::Left)
			.Text(FText::FromString("Select Sound Wave file: "))
			.ToolTipText(FText::FromString("The Sound Wave whose subtitles to override."))
				
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
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SCheckBox)
				.OnCheckStateChanged(this, &SSubImpWindow::OnStringTableCheckboxStateChanged)
				.IsChecked(this, &SSubImpWindow::GetStringTableCheckBoxState)
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(STextBlock)
				.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
				.Justification(ETextJustify::Left)
				.Text(FText::FromString("Assign subtitles as String Table keys?"))
				.ToolTipText(FText::FromString("Should the subtitles be treated as keys defined in a String Table?"))
			]
		]

		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 0.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.Visibility(this, &SSubImpWindow::GetStringTableSettingsVisibility)
			.BorderImage(FAppStyle::Get().GetBrush("Brushes.Background"))
			.Padding(5.0f, 0.0f, 5.0f, 5.0f)
			[
				SNew(SVerticalBox)

				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 5.0f)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				[
					SNew(STextBlock)
					.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
					.Justification(ETextJustify::Left)
					.Text(FText::FromString("Select String Table: "))
					.ToolTipText(FText::FromString("The String Table from which to get the keys."))
				]
					
				+SVerticalBox::Slot()
				[
					SNew(SObjectPropertyEntryBox)
					.ThumbnailPool(UThumbnailManager::Get().GetSharedThumbnailPool())
					.DisplayThumbnail(true)
					.DisplayBrowse(true)
					.EnableContentPicker(true)
					.AllowedClass(UStringTable::StaticClass())
					.ObjectPath(this, &SSubImpWindow::GetSelectedStringTablePath)
					.OnObjectChanged(this, &SSubImpWindow::OnStringTableSelected)
				]
			]
		]

		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SCheckBox)
				.OnCheckStateChanged(this, &SSubImpWindow::OnApplyEndSubTagStatChanged)
				.IsChecked(this, &SSubImpWindow::GetApplyEndSubTagCheckBoxState)
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(STextBlock)
				.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
				.Justification(ETextJustify::Left)
				.Text(FText::FromString("Apply End Sub Tag?"))
				.ToolTipText(FText::FromString("End Sub Tag adds a new subtitle entry (as a string tag) to differentiate when a subtitle ends. Tag is only applied if the the time difference between two subtitles exceeds the given timeout."))
			]
		]
		
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 0.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.Visibility(this, &SSubImpWindow::GetEndSubTagSettingsVisibility)
			.BorderImage(FAppStyle::Get().GetBrush("Brushes.Background"))
			.Padding(5.0f, 0.0f, 5.0f, 5.0f)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 5.0f)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				[
					SNew(STextBlock)
					.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
					.Justification(ETextJustify::Left)
					.Text(FText::FromString("End Sub Tag: "))
					.ToolTipText(FText::FromString("The string tag to apply as a subtitle cue entry."))
				]

				+SVerticalBox::Slot()
				.Padding(0.0f, 3.0f)
				[
					SNew(SEditableTextBox)
					.IsReadOnly(false)
					.HintText(FText::FromString("End Sub Tag"))
					.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
					.Text(this, &SSubImpWindow::GetEndSubTag)
					.OnTextCommitted(this, &SSubImpWindow::EndSubTagTextBlockCommitted)
				]
				
				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 5.0f)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				[
					SNew(STextBlock)
					.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
					.Justification(ETextJustify::Left)
					.Text(FText::FromString("End Sub Tag Timeout (in seconds): "))
					.ToolTipText(FText::FromString("The timeout between two subtitles that needs to be exceeded in order for the tag to be applied."))
				]

				+SVerticalBox::Slot()
				.Padding(0.0f, 3.0f)
				[
					SNew(SSpinBox<float>)
					.MinSliderValue(0.5f)
					.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
					.Value(this, &SSubImpWindow::GetEndSubTagTimeout)
					.OnValueCommitted(this, &SSubImpWindow::EndSubTagTimeoutNumericCommitted)
				]
			]
		]
		
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			SNew(SBox)
			.HeightOverride(40.0f)
			[
				SNew(SButton)
				.IsEnabled(this, &SSubImpWindow::GetIsGenerateButtonEnabled)
				.OnClicked(this, &SSubImpWindow::DoTheSubImp)
				[
					SNew(SBox)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::FromString("sub-imp Generate Subtitles"))
						.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
						.Justification(ETextJustify::Center)
					]
				]
			]
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

	if (FoundFile && !ReadTextFiles.IsEmpty())
	{
		const FString FilePath = ReadTextFiles[0];
		LoadedFileString = FilePath;
	}
	
	return FReply::Handled();
}

FReply SSubImpWindow::DoTheSubImp()
{
	if(!GEditor)
		return FReply::Handled();

	GenerateSubtitleCueArrayFromReadFile();
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

void SSubImpWindow::ResetSubImp()
{
	LoadedFileString = OPEN_FILE_HINT_STRING;
	GeneratedSubtitleInfo.Empty();
	
	SelectedSoundWave = nullptr;
	SelectedSoundWavePath = FString();

	bApplyEndSubTag = false;
	bUseStringTableForSubtitles = false;

	EndSubTag = END_SUB_DEFAULT_TAG;
	EndSubTagTimeout = END_SUB_MINIMUM_TIMEOUT;
	
	SelectedStringTable = nullptr;
	SelectedStringTablePath = FString();
}

void SSubImpWindow::GenerateSubtitleCueArrayFromReadFile()
{
	GeneratedSubtitleInfo.Empty();
	
	if(!IsLoadedPathStringValid())
		return;
	
	TArray<FString> LoadedText;
	int SubtitleIndex = -1;
	float CachedSubtitleEndTime = 0.0f;
	FSubtitleCue SubtitleCue = FSubtitleCue();
	
	FFileHelper::LoadFileToStringArray(LoadedText, *LoadedFileString);
	const int LastIndex = LoadedText.Num() - 1;
	
	for (int i = 0; i <= LastIndex; i++)
	{
		FString Line = LoadedText[i];
		ESubImpLineType LineType = ESubImpLineType::EmptyLine;
		
		if(!Line.IsEmpty())
		{
			bool bPushSubtitleCue = false;
			
			if(Line.IsNumeric())
				LineType = ESubImpLineType::SubtitleIndex;
			
			if(Line.Contains(SUBTITLE_TIME_DELIMITER))
				LineType = ESubImpLineType::SubtitleTime;

			if(LineType == ESubImpLineType::EmptyLine)
				LineType = ESubImpLineType::SubtitleText;

			switch(LineType)
			{
			case ESubImpLineType::SubtitleIndex:
			{
				const int NewSubtitleIndex = FCString::Atoi(*Line);
				if (NewSubtitleIndex > SubtitleIndex)
				{
					SubtitleIndex = NewSubtitleIndex;
					bPushSubtitleCue = true;
				}
				break;
			}
			case ESubImpLineType::SubtitleTime:
			{
				FString LeftString, RightString;
				Line.Split(SUBTITLE_TIME_DELIMITER, &LeftString, &RightString);

				const float CurrentSubtitleStartTime = GetTotalSecondsFromTimespanString(LeftString);

				if (bApplyEndSubTag
				&& !GetEndSubTag().IsEmpty()
				&& (CurrentSubtitleStartTime - CachedSubtitleEndTime) >= GetEndSubTagTimeout())
				{
					FSubtitleCue EmptySubtitleCue = FSubtitleCue();
					EmptySubtitleCue.Text = GetEndSubTag();
					EmptySubtitleCue.Time = CachedSubtitleEndTime;
				
					GeneratedSubtitleInfo.Add(EmptySubtitleCue);
				}
				
				SubtitleCue.Time = CurrentSubtitleStartTime;
				CachedSubtitleEndTime = GetTotalSecondsFromTimespanString(RightString);
				break;
			}
			case ESubImpLineType::SubtitleText:
			{
				if(bUseStringTableForSubtitles)
				{
					FText SubtitleText = FText::FromString("MISSING STRING TABLE ENTRY");
					
					if(SelectedStringTable != nullptr)
					{
						const FName TableId = SelectedStringTable->GetStringTableId();
						if(TableId.IsValid())
						{
							SubtitleText = FText::FromStringTable(TableId, Line);
						}
					}
					
					SubtitleCue.Text = SubtitleText;
					break;
				}
					
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
				break;
			}
			default:
				break;
			}

			// Force subtitle update, to ensure the final subs go into the array
			if(i == LastIndex)
				bPushSubtitleCue = true;

			if(bPushSubtitleCue)
			{
				if(!SubtitleCue.Text.IsEmpty() && SubtitleCue.Time >= 0.0f)
				{
					GeneratedSubtitleInfo.Add(SubtitleCue);
					SubtitleCue = FSubtitleCue();
				}
			}
		}
	}
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
	return (SelectedSoundWave != nullptr) && IsLoadedPathStringValid();
}

bool SSubImpWindow::IsLoadedPathStringValid() const
{
	return !(LoadedFileString.Equals(OPEN_FILE_HINT_STRING, ESearchCase::IgnoreCase) || LoadedFileString.IsEmpty());
}

FText SSubImpWindow::GetLoadedFileStringAsText() const
{
	return FText::FromString(LoadedFileString);
}

FText SSubImpWindow::GetLoadedFileTooltipText() const
{
	if(IsLoadedPathStringValid())
		return GetLoadedFileStringAsText();

	return FText::GetEmpty();
}

FString SSubImpWindow::GetSelectedSoundWavePath() const
{
	return SelectedSoundWavePath;
}

void SSubImpWindow::OnStringTableSelected(const FAssetData& InStringTable)
{
	if(InStringTable.IsValid())
	{
		UStringTable* StringTable = reinterpret_cast<UStringTable*>(InStringTable.GetAsset());

		if(StringTable == nullptr)
			return;

		SelectedStringTable = StringTable;
		SelectedStringTablePath = InStringTable.GetObjectPathString();
	}
}

FString SSubImpWindow::GetSelectedStringTablePath() const
{
	return SelectedStringTablePath;
}

void SSubImpWindow::OnStringTableCheckboxStateChanged(ECheckBoxState bIsChecked)
{
	bUseStringTableForSubtitles = (bIsChecked == ECheckBoxState::Checked);
}

ECheckBoxState SSubImpWindow::GetStringTableCheckBoxState() const
{
	return bUseStringTableForSubtitles ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

EVisibility SSubImpWindow::GetStringTableSettingsVisibility() const
{
	return bUseStringTableForSubtitles ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed;
}

void SSubImpWindow::OnApplyEndSubTagStatChanged(ECheckBoxState bIsChecked)
{
	bApplyEndSubTag = (bIsChecked == ECheckBoxState::Checked);
}

ECheckBoxState SSubImpWindow::GetApplyEndSubTagCheckBoxState() const
{
	return bApplyEndSubTag ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; 
}

EVisibility SSubImpWindow::GetEndSubTagSettingsVisibility() const
{
	return bApplyEndSubTag ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed;
}

void SSubImpWindow::EndSubTagTextBlockCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	EndSubTag = Text;
}

FText SSubImpWindow::GetEndSubTag() const
{
	return EndSubTag;
}

void SSubImpWindow::EndSubTagTimeoutNumericCommitted(float Value, ETextCommit::Type CommitType)
{
	EndSubTagTimeout = FMath::Max(END_SUB_MINIMUM_TIMEOUT, Value);
}

float SSubImpWindow::GetEndSubTagTimeout() const
{
	return EndSubTagTimeout;
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

#define WINDOW_SIZE FVector2D(350.0f, 550.0f)
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
		.SupportsMinimize(true)
		.SizingRule(ESizingRule::Autosized)
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

