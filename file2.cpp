//◦ Playrix ◦
#include "stdafx.h"
#include "ExpeditionTutorialActions.h"

#include "Common/Game/ActionsQueue/ActionsQueue.h"
#include "GameFeatures/GameEvents/Expedition/Common/IExpeditionTutorialController.h"
#include "GameFeatures/GameEvents/Expedition/utils/ExpeditionUtils.h"
#include <Codegen/ExpeditionTutorialActions.generated.hpp>

#include "Game/Tutorial/TutorialManager.h"
#include "Game/Tutorial/TutorialObject.h"
#include "GameFeatures/GameEvents/Expedition/Components/ExpeditionTutorialComponent.h"

namespace Expedition
{
	void StartTutorialAction::Execute(const Visual::VarsContext&) noexcept
	{
		if (auto delegate = GetActualDelegate())
		{
			if (auto tutorialController = delegate->GetTutorialController())
			{
				tutorialController->StartTutorial(_addAction, _lockAll);
			}
		}
	}

	void StopTutorialAction::Execute(const Visual::VarsContext&) noexcept
	{
		if (auto delegate = GetActualDelegate())
		{
			if (auto tutorialController = delegate->GetTutorialController())
			{
				tutorialController->StopTutorial();
			}
		}
	}

	void StartTutorialQueueAction::Execute(const Visual::VarsContext&) noexcept
	{
		if (auto delegate = GetActualDelegate())
		{
			if (auto tutorialController = delegate->GetTutorialController())
			{
				tutorialController->StartTutorialAction();
			}
		}
	}

	void StopTutorialQueueAction::Execute(const Visual::VarsContext&) noexcept
	{
		if (auto delegate = GetActualDelegate())
		{
			if (auto tutorialController = delegate->GetTutorialController())
			{
				tutorialController->StopTutorialAction();
			}
		}
	}

	void SetTutorialShownTutorialAction::Execute(const Visual::VarsContext&) noexcept
	{
		if (auto delegate = GetActualDelegate())
		{
			if (auto tutorialController = delegate->GetTutorialController())
			{
				tutorialController->SetTutorialShown(_tutorialId, _shown);
			}
		}
	}

	void SetObjectTappableTutorialAction::Execute(const Visual::VarsContext&) noexcept
	{
		if (auto delegate = GetActualDelegate())
		{
			if (auto tutorialController = delegate->GetTutorialController())
			{
				tutorialController->SetObjectTappable(_objectId, _tappable);
			}
		}
	}

	void SimulateObjectTapTutorialAction::Execute(const Visual::VarsContext&) noexcept
	{
		if (auto delegate = GetActualDelegate())
		{
			if (auto tutorialController = delegate->GetTutorialController())
			{
				tutorialController->SimulateObjectTap(_objectId);
			}
		}
	}

	void SetAutoTooltipsEnabledTutorialAction::Execute(const Visual::VarsContext&) noexcept
	{
		if (auto delegate = GetActualDelegate())
		{
			if (auto tutorialController = delegate->GetTutorialController())
			{
				tutorialController->SetAutoTooltipsEnabled(_enabled);
			}
		}
	}

	void SetInterfacePressableTutorialAction::Execute(const Visual::VarsContext&) noexcept
	{
		if (auto delegate = GetActualDelegate())
		{
			if (auto tutorialController = delegate->GetTutorialController())
			{
				tutorialController->SetInterfacePressable(_pressable);
			}
		}
	}

	void SetCameraLockTutorialAction::Execute(const Visual::VarsContext&) noexcept
	{
		if (auto delegate = GetActualDelegate())
		{
			if (auto tutorialController = delegate->GetTutorialController())
			{
				tutorialController->SetCameraLocked(_lock);
			}
		}
	}

	void CameraMoveTutorialAction::Execute(const Visual::VarsContext&) noexcept
	{
		if (auto delegate = GetActualDelegate())
		{
			if (auto tutorialController = delegate->GetTutorialController())
			{
				tutorialController->SetCameraPosition(_coord, _time, _linear);
			}
		}
	}

	void SetCharacterLockTutorialAction::Execute(const Visual::VarsContext&) noexcept
	{
		if (auto delegate = GetActualDelegate())
		{
			if (auto tutorialController = delegate->GetTutorialController())
			{
				tutorialController->SetCharacterInputLocked(_lock);
			}
		}
	}

	void AdvancedStartTutorialAction::TutorialObject::Exclude()
	{
		if (_id.empty())
		{
			Assert2(false, "Error! Tutorial object ID is empty. Check tutorial action settings.");
			return;
		}

		if (const auto& manager = Game::Tutorial::TutorialManager::GetExistingInstance())
		{
			manager->GetTutorialObject(_id)->Unlock();
		}
	}

	void AdvancedStartTutorialAction::GardenObject::Exclude()
	{
		if (_id.empty())
		{
			Assert2(false, "Error! Garden object ID is empty. Check tutorial action settings.");
			return;
		}

		auto* controller = AdvancedStartTutorialAction::GetTutorialController();
		if (!controller)
		{
			return;
		}

		controller->SetObjectTappable(_id, true);
	}

	ITutorialController* AdvancedStartTutorialAction::GetTutorialController()
	{
		const auto* delegate = GetActualDelegate();
		if (!delegate)
		{
			Assert2(false, "Error! Expedition delegate unavailable.");
			return nullptr;
		}

		auto* controller = delegate->GetTutorialController();
		if (!controller)
		{
			Assert2(false, "Error! Expedition tutorial controller unavailable. Check event settings.");
			return nullptr;
		}

		return controller;
	}

	void AdvancedStartTutorialAction::Execute(const Visual::VarsContext&) noexcept
	{
		auto* controller = AdvancedStartTutorialAction::GetTutorialController();
		if (!controller)
		{
			return;
		}

		// См. Expedition::TutorialComponent::StartTutorial. Действие в очередь добавится ниже,
		// а второй аргумент нужен, чтобы заблокировать объекты дома, т.к. снаружи это не сделать.
		controller->StartTutorial(false, Core::IsEnumFlagSet(_locks, Locks::Garden));

		if (Core::IsEnumFlagSet(_locks, Locks::Queue))
		{
			if (!Game::ActionsQueue::Instance().ActionExist(TutorialComponent::EXPEDITION_TUTORIAL_ACTION_ID))
			{
				controller->StartTutorialAction();
			}
		}

		controller->SetCameraLocked(Core::IsEnumFlagSet(_locks, Locks::Camera));
		controller->SetInterfacePressable(!Core::IsEnumFlagSet(_locks, Locks::Interface));
		controller->SetCharacterInputLocked(Core::IsEnumFlagSet(_locks, Locks::Character));
		controller->SetAutoTooltipsEnabled(!Core::IsEnumFlagSet(_locks, Locks::Tooltips));

		if (const auto& manager = Game::Tutorial::TutorialManager::GetExistingInstance())
		{
			if (Core::IsEnumFlagSet(_locks, Locks::Objects))
			{
				manager->LockAll();
			}
			else
			{
				manager->UnlockAll();
			}
		}

		for (const auto& exclusion : _exclusions)
		{
			exclusion->Exclude();
		}
	}

	void AdvancedStopTutorialAction::Execute(const Visual::VarsContext&) noexcept
	{
		auto* controller = AdvancedStartTutorialAction::GetTutorialController();
		if (!controller)
		{
			return;
		}

		controller->StopTutorial();

		if (const auto& manager = Game::Tutorial::TutorialManager::GetExistingInstance())
		{
			manager->UnlockAll();
		}
	}
}