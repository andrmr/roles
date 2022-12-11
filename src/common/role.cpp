#include "role.h"

using namespace tser;

Role::Role(Name name, Uuid uuid)
	: name {std::move(name)}
	, uuid {std::move(uuid)}
{
}

bool
Role::has_subroles() const
{
	return !sub_roles.empty();
}

bool
Role::has_subrole(Uuid subrole) const
{
	return sub_roles.contains(subrole);
}

bool
Role::add_subrole(Uuid subrole)
{
	auto [_, ok] = sub_roles.insert(subrole);
	return ok;
}

bool
Role::rem_subrole(Uuid subrole)
{
	return 1 == sub_roles.erase(subrole);
}

void
tser::to_json(nlohmann::json& j, const Role& role)
{
	j = nlohmann::json{
		{"name", role.name},
		{"id", role.uuid},
	};

	if (!role.sub_roles.empty()) {
		j["includedRoles"] = role.sub_roles;
	}
}

void
tser::from_json(const nlohmann::json& j, Role& role)
{
	j.at("name").get_to(role.name);
	j.at("id").get_to(role.uuid);

	if (j.count("includedRoles") != 0) {
		j.at("includedRoles").get_to(role.sub_roles);
	}
}
