#pragma once

#include <expected>
#include <optional>
#include <unordered_map>

#include "role.h"

namespace tser
{
	/// @brief Errors which may occur during Repository operations
	enum class RepositoryErr
	{
		OK,
		UNKNOWN_ERR,
		ILLEGAL_OP,
		ROLE_ALREADY_EXISTS,
		ROLE_NOT_FOUND,
		ROLE_HAS_SUBROLE,
	};

	/// @brief Return which conveys either a value or an error
	template <typename T>
	using RepositoryResult = std::expected<T, RepositoryErr>;

	/// @brief Represents a Repository with the associated operations
	class IRepository
	{
	public:
		/// @brief Attempts to add a new role to the Repository
		virtual RepositoryErr add_role(Role role) = 0;

		/// @brief Returns existing roles
		virtual std::unordered_map<Role::Uuid, Role> roles() const = 0;
		
		/// @brief Returns a list of roles which include a given subrole
		virtual RepositoryResult<std::optional<std::unordered_set<Role::Uuid>>> dependencies(const Role::Uuid& subrole) const = 0;
		
		/// @brief Attempts to include a subrole into a given role
		virtual RepositoryErr include_role(const Role::Uuid& role, const Role::Uuid& subrole) = 0;
		
		/// @brief Attempts to exclude a subrole from a given role
		virtual RepositoryErr exclude_role(const Role::Uuid& role, const Role::Uuid& subrole) = 0;

		/// @brief Checks if a role exists
		virtual bool is_valid_role(const Role::Uuid& role) const = 0;

		virtual ~IRepository() = default;

		/// @brief Provides error-to-string mapping for Repository errors
		const auto& err_to_str(RepositoryErr e) const
		{
			return err_map.at(e);
		}

	private:
		using enum RepositoryErr;
		inline static const std::unordered_map<RepositoryErr, std::string_view> err_map {
			{UNKNOWN_ERR, "unknown error"},
			{ILLEGAL_OP, "illegal operation"},
			{ROLE_ALREADY_EXISTS, "role already exists"},
			{ROLE_NOT_FOUND, "role not found"},
			{ROLE_HAS_SUBROLE, "role has subrole"},
		};
	};
}