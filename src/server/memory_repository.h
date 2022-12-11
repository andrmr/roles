#pragma once

#include <shared_mutex>

#include "repository.h"

namespace tser
{
	/// @brief in-memory implementation for the Repository interface
	class MemoryRepository : public IRepository
	{
	private:
		// todo: separate mutexes for each container
		std::shared_mutex mutable mutex;
		std::unordered_map<Role::Uuid, Role> repository;
		std::unordered_map<Role::Uuid, std::unordered_set<Role::Uuid>> deps;

	public:
		MemoryRepository() = default;

		virtual RepositoryErr add_role(Role role) override;

		virtual std::unordered_map<Role::Uuid, Role> roles() const override;

		virtual RepositoryResult<std::optional<std::unordered_set<Role::Uuid>>> dependencies(const Role::Uuid& subrole) const override;

		virtual RepositoryErr include_role(const Role::Uuid& role, const Role::Uuid& subrole) override;

		virtual RepositoryErr exclude_role(const Role::Uuid& role, const Role::Uuid& subrole) override;

		virtual bool is_valid_role(const Role::Uuid& role) const override;
	};
}