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
							.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
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
				.Text(FText::FromString("Assign subtitles to String Table keys?"))
			]
		]

		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.Visibility(this, &SSubImpWindow::GetStringTableSettingsVisibility)
			.BorderImage(FAppStyle::Get().GetBrush("Brushes.Background"))
			.Padding(2.0f, 5.0f, 2.0f, 5.0f)
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
					.Text(FText::FromString("Select String Table: "))
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
			]
		]
		
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f)
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

				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 5.0f)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				[
					SNew(STextBlock)
					.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
					.Justification(ETextJustify::Left)
					.Text(FText::FromString("Sub Imp Tag Timeout (in seconds): "))
					.ToolTipText(FText::FromString("Description"))
				]

				+SVerticalBox::Slot()
				[
					SNew(SSpinBox<float>)
					.MinSliderValue(0.5f)
					.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
					.Value(this, &SSubImpWindow::GetEndSubTagTimeout)
					.OnValueCommitted(this, &SSubImpWindow::EndSubTagTimeoutNumericCommitted)
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
					.Text(FText::FromString("Sub Imp Tag Type: "))
					.ToolTipText(FText::FromString("Description"))
				]

				+SVerticalBox::Slot()
				[
					SNew(SEditableTextBox)
					.IsReadOnly(false)
					.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
					.Text(this, &SSubImpWindow::GetEndSubTag)
					.OnTextCommitted(this, &SSubImpWindow::EndSubTagTextBlockCommitted)
				]
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
	EndSubTagTimeout = FMath::Max(0.5f, Value);
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
		.SupportsMinimize(false)
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

