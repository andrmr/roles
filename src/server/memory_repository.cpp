#include "memory_repository.h"

using namespace tser;

RepositoryErr
MemoryRepository::add_role(Role role)
{
	std::unique_lock lk{ mutex };
	if (repository.contains(role.uuid)) {
		return RepositoryErr::ROLE_ALREADY_EXISTS;
	}
	else {
		repository.insert({ role.uuid, std::move(role) });
	}

	return RepositoryErr::OK;
}

std::unordered_map<Role::Uuid, Role>
MemoryRepository::roles() const
{
	std::shared_lock lk{ mutex };
	return repository;
}

RepositoryResult<std::optional<std::unordered_set<Role::Uuid>>>
MemoryRepository::dependencies(const Role::Uuid& subrole) const
{
	std::shared_lock lk{ mutex };
	if (!repository.contains(subrole)) {
		return std::unexpected(RepositoryErr::ROLE_NOT_FOUND);
	}

	if (deps.contains(subrole))
	{
		return deps.at(subrole);
	}

	return std::nullopt;
}

RepositoryErr
MemoryRepository::include_role(const Role::Uuid& role, const Role::Uuid& subrole)
{
	if (role == subrole) {
		return RepositoryErr::ILLEGAL_OP;
	}

	{
		std::shared_lock lk{ mutex };
		if (!repository.contains(role) || !repository.contains(subrole)) {
			return RepositoryErr::ROLE_NOT_FOUND;
		}

		if (repository.at(subrole).has_subroles()) {
			return RepositoryErr::ROLE_HAS_SUBROLE;
		}
	}

	std::scoped_lock lk{ mutex };
	if (!repository.contains(role)) {
		return RepositoryErr::ROLE_NOT_FOUND;
	}

	deps[subrole].insert(role);
	return repository[role].add_subrole(subrole) ? RepositoryErr::OK : RepositoryErr::UNKNOWN_ERR;
}

RepositoryErr
MemoryRepository::exclude_role(const Role::Uuid& role, const Role::Uuid& subrole)
{
	{
		std::shared_lock lk{ mutex };
		if (!repository.contains(role)) {
			return RepositoryErr::ROLE_NOT_FOUND;
		}
	}

	std::scoped_lock lk{ mutex };
	if (!repository.contains(role)) {
		return RepositoryErr::ROLE_NOT_FOUND;
	}

	deps[subrole].erase(role);
	return repository.at(role).rem_subrole(subrole) ? RepositoryErr::OK : RepositoryErr::UNKNOWN_ERR;
}

bool
MemoryRepository::is_valid_role(const Role::Uuid& role) const
{
	std::shared_lock lk{ mutex };
	return repository.contains(role);
}