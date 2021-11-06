// Copyright 2020-2021 Maksym Maisak. All Rights Reserved.

#include "HTNEditorToolbarBuilder.h"
#include "ToolMenus/Public/ToolMenuDelegates.h"
#include "WorkflowOrientedApp/SModeWidget.h"
#include "Widgets/Layout/SSpacer.h"

#include "HTNEditor.h"
#include "HTNDebugger.h"
#include "HTNCommands.h"

#define LOCTEXT_NAMESPACE "HTNEditorToolbarBuilder"

namespace
{
	class SHTNModeSeparator : public SBorder
	{
	public:
		SLATE_BEGIN_ARGS(SHTNModeSeparator) {}
		SLATE_END_ARGS()

		void Construct(const FArguments& InArg)
		{
			SBorder::Construct(
				SBorder::FArguments()
				.BorderImage(FEditorStyle::GetBrush("BlueprintEditor.PipelineSeparator"))
				.Padding(0.0f)
			);
		}

		// SWidget interface
		virtual FVector2D ComputeDesiredSize(float) const override
		{
			const float Height = 20.0f;
			const float Thickness = 16.0f;
			return FVector2D(Thickness, Height);
		}
		// End of SWidget interface
	};
}

void FHTNEditorToolbarBuilder::AddModesToolbar(TSharedPtr<FExtender> Extender)
{
	check(HTNEditorWeakPtr.IsValid());
	const TSharedPtr<FHTNEditor> HTNEditor = HTNEditorWeakPtr.Pin();

	Extender->AddToolBarExtension(
		TEXT("Asset"),
		EExtensionHook::After,
		HTNEditor->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FHTNEditorToolbarBuilder::FillModesToolbar)
	);
}

void FHTNEditorToolbarBuilder::AddDebuggerToolbar(TSharedPtr<FExtender> Extender)
{
	check(HTNEditorWeakPtr.IsValid());
	const TSharedPtr<FHTNEditor> HTNEditor = HTNEditorWeakPtr.Pin();

	const TSharedPtr<FExtender> ToolbarExtender = MakeShared<FExtender>();
	ToolbarExtender->AddToolBarExtension(
		TEXT("Asset"),
		EExtensionHook::After, 
		HTNEditor->GetToolkitCommands(), 
		FToolBarExtensionDelegate::CreateSP(this, &FHTNEditorToolbarBuilder::FillDebuggerToolbar)
	);
	HTNEditor->AddToolbarExtender(ToolbarExtender);
}

void FHTNEditorToolbarBuilder::FillModesToolbar(FToolBarBuilder& ToolbarBuilder)
{
	check(HTNEditorWeakPtr.IsValid());
	const TSharedPtr<FHTNEditor> HTNEditor = HTNEditorWeakPtr.Pin();

	const TAttribute<FName> GetActiveMode(HTNEditor.ToSharedRef(), &FHTNEditor::GetCurrentMode);
	const FOnModeChangeRequested SetActiveMode = FOnModeChangeRequested::CreateSP(HTNEditor.ToSharedRef(), &FHTNEditor::SetCurrentMode);

	// Left side padding
	HTNEditor->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));

	// HTN Mode
	HTNEditor->AddToolbarWidget(
		SNew(SModeWidget, FHTNEditor::GetLocalizedModeDescription(FHTNEditor::HTNMode), FHTNEditor::HTNMode)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		.CanBeSelected(HTNEditor.Get(), &FHTNEditor::CanAccessHTNMode)
		.ToolTipText(LOCTEXT("HTNModeButtonTooltip", "Switch to HTN Mode"))
		.IconImage(FEditorStyle::GetBrush("BTEditor.SwitchToBehaviorTreeMode"))
#if ENGINE_MAJOR_VERSION <= 4
		.SmallIconImage(FEditorStyle::GetBrush("BTEditor.SwitchToBehaviorTreeMode.Small"))
#endif
	);

	HTNEditor->AddToolbarWidget(SNew(SHTNModeSeparator));

	// Blackboard mode
	HTNEditor->AddToolbarWidget(
		SNew(SModeWidget, FHTNEditor::GetLocalizedModeDescription(FHTNEditor::BlackboardMode), FHTNEditor::BlackboardMode)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		.CanBeSelected(HTNEditor.Get(), &FHTNEditor::CanAccessBlackboardMode)
		.ToolTipText(LOCTEXT("BlackboardModeButtonTooltip", "Switch to Blackboard Mode"))
		.IconImage(FEditorStyle::GetBrush("BTEditor.SwitchToBlackboardMode"))
#if ENGINE_MAJOR_VERSION <= 4
		.SmallIconImage(FEditorStyle::GetBrush("BTEditor.SwitchToBlackboardMode.Small"))
#endif
	);

	// Right side padding
	HTNEditor->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));
}

void FHTNEditorToolbarBuilder::FillDebuggerToolbar(FToolBarBuilder& ToolbarBuilder)
{
	check(HTNEditorWeakPtr.IsValid());
	const TSharedPtr<FHTNEditor> HTNEditor = HTNEditorWeakPtr.Pin();
	const TSharedPtr<FHTNDebugger> Debugger = HTNEditor->GetDebugger();
	if (!Debugger.IsValid() || !Debugger->IsDebuggerReady())
	{
		return;
	}

	const TSharedRef<SWidget> SelectionBox = SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 4.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("SelectDebugActorTitle", "Debugged actor"))
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SNew(SComboButton)
			.OnGetMenuContent(Debugger.ToSharedRef(), &FHTNDebugger::GetActorsMenu)
			.ButtonContent()
			[
				SNew(STextBlock)
				.ToolTipText(LOCTEXT("SelectDebugActor", "Pick actor to debug"))
				.Text(Debugger.ToSharedRef(), &FHTNDebugger::GetCurrentActorDescription)
			]
		];

	/*
	ToolbarBuilder.BeginSection("CachedState");
	{
		ToolbarBuilder.AddToolBarButton(FHTNDebuggerCommands::Get().BackOver);
		ToolbarBuilder.AddToolBarButton(FHTNDebuggerCommands::Get().BackInto);
		ToolbarBuilder.AddToolBarButton(FHTNDebuggerCommands::Get().ForwardInto);
		ToolbarBuilder.AddToolBarButton(FHTNDebuggerCommands::Get().ForwardOver);
		ToolbarBuilder.AddToolBarButton(FHTNDebuggerCommands::Get().StepOut);
	}
	ToolbarBuilder.EndSection();
	*/
	ToolbarBuilder.BeginSection(TEXT("World"));
	{
		ToolbarBuilder.AddToolBarButton(FHTNDebuggerCommands::Get().PausePlaySession);
		ToolbarBuilder.AddToolBarButton(FHTNDebuggerCommands::Get().ResumePlaySession);
		ToolbarBuilder.AddToolBarButton(FHTNDebuggerCommands::Get().StopPlaySession);
		ToolbarBuilder.AddSeparator();
		ToolbarBuilder.AddWidget(SelectionBox);
	}
	ToolbarBuilder.EndSection();
}

#undef LOCTEXT_NAMESPACE