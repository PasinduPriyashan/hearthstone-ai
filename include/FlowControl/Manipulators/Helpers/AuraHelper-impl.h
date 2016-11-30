#pragma once

#include <assert.h>
#include "FlowControl/Dispatchers/Auras.h"
#include "FlowControl/Manipulators/Helpers/AuraHelper.h"
#include "FlowControl/Manipulate.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			void AuraHelper::Update()
			{
				if (card_.GetAuraId() < 0) return; // no aura attached

				state::Cards::AuraAuxData & data = card_.GetMutableAuraAuxDataGetter().Get();

				std::unordered_set<state::CardRef> new_targets;
				Dispatchers::Auras::GetEligibles(card_.GetAuraId(), state_, card_ref_, card_, new_targets);

				for (auto it = data.applied_enchantments.begin(), it2 = data.applied_enchantments.end(); it != it2;)
				{
					auto new_target_it = new_targets.find(it->first);
					if (new_target_it != new_targets.end()) {
						// already applied
						new_targets.erase(new_target_it);
						++it;
					}
					else {
						// enchantments should be removed
						Dispatchers::Auras::RemoveFrom(card_.GetAuraId(), state_, flow_context_, card_ref_, card_, it->first, it->second);
						it = data.applied_enchantments.erase(it);
					}
				}

				for (auto new_target : new_targets) {
					// enchantments should be applied
					assert(data.applied_enchantments.find(new_target) == data.applied_enchantments.end());

					state::Cards::Enchantments::ContainerType::Identifier enchant_id;
					Dispatchers::Auras::ApplyOn(card_.GetAuraId(), state_, flow_context_, card_ref_, card_, new_target, enchant_id);
					data.applied_enchantments.insert(std::make_pair(new_target, std::move(enchant_id)));
				}
			}

		}
	}
}