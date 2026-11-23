#pragma once

namespace GameTypes
{
	/**
	 * Defines active input context modes for routing player input actions.
	 */
	UENUM(BlueprintType)
	enum class EInputType : uint8
	{
		/** Gameplay input routing (character locomotion, camera look, interaction). */
		GamePlay,

		/** UI navigation input routing (menus, dialogs, button focus). */
		UI
	};
}
