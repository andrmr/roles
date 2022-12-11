#pragma once

// json
#include <nlohmann/json.hpp>

// std
#include <string>
#include <unordered_set>

namespace tser
{
	/// @brief Models the concept of Role, and can be used as DTO
	class Role
	{
	public:
		// todo: strong types
		using Name = std::string;
		using Uuid = std::string;

		Role() = default;

		Role(Name name, Uuid uuid);

		Name name;
		Uuid uuid;

		bool has_subroles() const;

		bool has_subrole(Uuid subrole) const;

		bool add_subrole(Uuid subrole);

		bool rem_subrole(Uuid subrole);

		auto operator<=>(const Role&) const = default;

	private:
		std::unordered_set<Uuid> sub_roles;

		// serde
		friend void to_json(nlohmann::json& j, const Role& role);
		friend void from_json(const nlohmann::json& j, Role& role);
	};
}