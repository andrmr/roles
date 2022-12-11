#include "memory_repository.h"
#include "server.h"

/// Compiled and tested with VS 2022 17.5 with /std:c++ latest

/// Async REST server, based on restinio for HTTP and nlohmann for JSON.
/// Requirements:
///			1. Produce a list of all the Roles and their included roles.
///			2. Include Role in another Role.
///			3. Delete an included role from a role.
///			4. Simulate the deletion of a role, in order to obtain a report of the roles
///			that are affected by this deletion(they have this role included).
///			5. When including / deleting a role to / from a role, a response about the status
///			of the operation should be sent.

int main()
{
	// using memory storage; can be switched to other implementations, e.g. persistent DB
	std::shared_ptr<tser::IRepository> storage = std::make_shared<tser::MemoryRepository>();
	auto server = tser::Server(storage);

	// blocking call, but requests are handled async
	// use ctrl+c to send shutdown signal
	// todo: take args from cmd line
	server.run("localhost", 8150);

	return 0;
}