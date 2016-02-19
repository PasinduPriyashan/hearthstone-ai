#include "game-engine/slot-index.h"
#include "game-engine/board.h"

namespace GameEngine {

	// return true if any
	inline bool SlotIndexHelper::MarkAttackableMinions(Board const& board, SlotIndex side, SlotIndexBitmap &bitmap)
	{
		bool ret = false;

		for (auto minion_iterator = board.object_manager.GetMinionsIteratorWithIndexAtBeginOfSide(side);
			!minion_iterator.IsEnd(); minion_iterator.GoToNext())
		{
			if (minion_iterator.GetMinion().Attackable()) {
				bitmap.SetOneTarget(minion_iterator.GetSlotIdx());
				ret = true;
			}
		}

		return ret;
	}

	// return true if any
	inline bool SlotIndexHelper::MarkTauntMinions(Board const& board, SlotIndex side, SlotIndexBitmap &bitmap)
	{
		bool ret = false;

		for (auto minion_iterator = board.object_manager.GetMinionsIteratorWithIndexAtBeginOfSide(side);
			!minion_iterator.IsEnd(); minion_iterator.GoToNext())
		{
			if (minion_iterator.GetMinion().stat.IsTaunt() && !minion_iterator.GetMinion().stat.IsStealth()) { // taunt with stealth will not act as taunt
				bitmap.SetOneTarget(minion_iterator.GetSlotIdx());
				ret = true;
			}
		}

		return ret;
	}

	// return true if any
	inline bool SlotIndexHelper::MarkMinionsWithoutStealth(Board const& board, SlotIndex side, SlotIndexBitmap &bitmap)
	{
		bool ret = false;

		for (auto minion_iterator = board.object_manager.GetMinionsIteratorWithIndexAtBeginOfSide(side);
			!minion_iterator.IsEnd(); minion_iterator.GoToNext())
		{
			if (!minion_iterator.GetMinion().stat.IsStealth()) {
				bitmap.SetOneTarget(minion_iterator.GetSlotIdx());
				ret = true;
			}
		}

		return ret;
	}

	// return true if any
	inline bool SlotIndexHelper::MarkMinions(Board const& board, SlotIndex side, SlotIndexBitmap &bitmap)
	{
		bool ret = false;

		for (auto minion_iterator = board.object_manager.GetMinionsIteratorWithIndexAtBeginOfSide(side);
			!minion_iterator.IsEnd(); minion_iterator.GoToNext())
		{
			bitmap.SetOneTarget(minion_iterator.GetSlotIdx());
			ret = true;
		}

		return ret;
	}

	inline SlotIndexBitmap SlotIndexHelper::GetTargets(SlotIndexHelper::TargetType type, GameEngine::Board const & board)
	{
		SlotIndexBitmap targets;

		switch (type)
		{
		case TARGET_TYPE_PLAYER_ATTACKABLE:
			if (board.object_manager.IsPlayerHeroAttackable()) {
				targets.SetOneTarget(SLOT_PLAYER_HERO);
			}
			MarkAttackableMinions(board, SLOT_PLAYER_SIDE, targets);
			break;

		case TARGET_TYPE_OPPONENT_ATTACKABLE:
			if (board.object_manager.IsOpponentHeroAttackable()) {
				targets.SetOneTarget(SLOT_OPPONENT_HERO);
			}
			MarkAttackableMinions(board, SLOT_OPPONENT_SIDE, targets);
			break;

		case TARGET_TYPE_PLAYER_CAN_BE_ATTACKED:
			if (MarkTauntMinions(board, SLOT_PLAYER_SIDE, targets)) break;

			targets.SetOneTarget(SLOT_PLAYER_HERO);
			MarkMinionsWithoutStealth(board, SLOT_PLAYER_SIDE, targets);
			break;

		case TARGET_TYPE_OPPONENT_CAN_BE_ATTACKED:
			if (MarkTauntMinions(board, SLOT_OPPONENT_SIDE, targets)) break;

			targets.SetOneTarget(SLOT_OPPONENT_HERO);
			MarkMinionsWithoutStealth(board, SLOT_OPPONENT_SIDE, targets);
			break;

		case TARGET_TYPE_PLAYER_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board, SLOT_PLAYER_SIDE, targets);
			break;

		case TARGET_TYPE_OPPONENT_MINIONS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board, SLOT_OPPONENT_SIDE, targets);
			break;

		case TARGET_TYPE_PLAYER_CHARACTERS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board, SLOT_PLAYER_SIDE, targets);
			targets.SetOneTarget(SLOT_PLAYER_HERO);
			break;

		case TARGET_TYPE_OPPONENT_CHARACTERS_TARGETABLE_BY_FRIENDLY_SPELL:
			// can also target stealth minions
			MarkMinions(board, SLOT_OPPONENT_SIDE, targets);
			targets.SetOneTarget(SLOT_OPPONENT_HERO);
			break;

		case TARGET_TYPE_PLAYER_MINIONS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board, SLOT_PLAYER_SIDE, targets);
			break;

		case TARGET_TYPE_OPPONENT_MINIONS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board, SLOT_OPPONENT_SIDE, targets);
			break;

		case TARGET_TYPE_PLAYER_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board, SLOT_PLAYER_SIDE, targets);
			targets.SetOneTarget(SLOT_PLAYER_HERO);
			break;

		case TARGET_TYPE_OPPONENT_CHARACTERS_TARGETABLE_BY_ENEMY_SPELL:
			// cannot target stealth minions
			MarkMinionsWithoutStealth(board, SLOT_OPPONENT_SIDE, targets);
			targets.SetOneTarget(SLOT_OPPONENT_HERO);
			break;

		default:
			throw std::runtime_error("unhandled case");
		}

		return targets;
	}

} // namespace GameEngine