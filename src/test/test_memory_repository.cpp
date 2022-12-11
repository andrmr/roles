#include "pch.h"

#include "..\common\role.h"
#include "..\common\role.cpp"
#include "..\server\memory_repository.h"
#include "..\server\memory_repository.cpp"

namespace tser_test {
	using namespace tser;

	TEST(MemoryRepository, AddRoleNew) {
		auto repo = MemoryRepository();

		auto result = repo.add_role(Role("role_0", "000"));
		ASSERT_EQ(result, RepositoryErr::OK);

		result = repo.add_role(Role("role_1", "001"));
		ASSERT_EQ(result, RepositoryErr::OK);
	}

	TEST(MemoryRepository, AddRoleExisting) {
		auto repo = MemoryRepository();

		auto result = repo.add_role(Role("role_0", "000"));
		ASSERT_EQ(result, RepositoryErr::OK);

		result = repo.add_role(Role("role_0", "000"));
		ASSERT_EQ(result, RepositoryErr::ROLE_ALREADY_EXISTS);

		result = repo.add_role(Role("role_1", "000"));
		ASSERT_EQ(result, RepositoryErr::ROLE_ALREADY_EXISTS);
	}

	TEST(MemoryRepository, GetRoles) {
		auto repo = MemoryRepository();

		Role roles[] = {
			Role("role_0", "000"),
			Role("role_1", "001"),
			Role("role_2", "002"),
		};

		for (const auto& role : roles) {
			repo.add_role(role);
		}

		const auto stored = repo.roles();
		for (const auto& role : roles) {
			ASSERT_EQ(role, stored.at(role.uuid));
		}
	}

	TEST(MemoryRepository, GetDependencies) {
		auto repo = MemoryRepository();

		Role roles[] = {
			Role("role_0", "000"),
			Role("role_1", "001"),
			Role("role_2", "002"),
		};

		for (const auto& role : roles) {
			repo.add_role(role);
		}

		repo.include_role(roles[0].uuid, roles[1].uuid);
		auto deps = repo.dependencies(roles[1].uuid);

		ASSERT_TRUE(deps.has_value() && deps.value().has_value());
		ASSERT_TRUE(deps.value().value().contains(roles[0].uuid));
	}

	TEST(MemoryRepository, IncludeRole) {
		auto repo = MemoryRepository();

		Role roles[] = {
			Role("role_0", "000"),
			Role("role_1", "001"),
			Role("role_2", "002"),
		};

		for (const auto& role : roles) {
			repo.add_role(role);
		}

		auto result = repo.include_role(roles[0].uuid, roles[1].uuid);
		ASSERT_EQ(result, RepositoryErr::OK);

		auto stored = repo.roles();
		ASSERT_TRUE(stored[roles[0].uuid].has_subrole(roles[1].uuid));

		result = repo.include_role(roles[0].uuid, roles[1].uuid);
		ASSERT_NE(result, RepositoryErr::OK) << "should fail when adding duplicate subrole";
	}

	TEST(MemoryRepository, ExcludeRole) {
		auto repo = MemoryRepository();

		Role roles[] = {
			Role("role_0", "000"),
			Role("role_1", "001"),
			Role("role_2", "002"),
		};

		for (const auto& role : roles) {
			repo.add_role(role);
		}

		repo.include_role(roles[0].uuid, roles[1].uuid);
		auto stored = repo.roles();
		ASSERT_TRUE(stored[roles[0].uuid].has_subrole(roles[1].uuid)) << "should contain sub-role";

		repo.exclude_role(roles[0].uuid, roles[1].uuid);
		stored = repo.roles();
		ASSERT_FALSE(stored[roles[0].uuid].has_subrole(roles[1].uuid)) << "should not contain sub-role";
	}
}