#pragma once

// std
#include <memory>
#include <string>

// restinio
#include <restinio/router/express.hpp>

// proj
#include "repository.h"

namespace tser
{
	/// @brief Encapsulates a request/response router and handles the calls to the storage provider
	class Server
	{
	public:
		/// @brief Creates an inactive server
		/// @param repository - the storage provider to be used for operations
		Server(std::shared_ptr<IRepository> repository);

		/// @brief Starts the server blockingly, however requests are handled async
		/// @param address - the address to bind
		/// @param port - the port to bind
		void run(std::string address, uint16_t port);

	private:
		// helpers
		enum class Verb
		{
			GET,
			POST,
			PUT,
		};

		void add_all_paths();
		void add_path(Verb verb, std::string_view path, auto&& handler);

		// handlers
		auto prepare_response(auto&& response);
		std::string handle_add_role(const auto& req, const auto& par);
		std::string handle_get_roles(const auto& req, const auto& par);
		std::string handle_get_simulation(const auto& req, const auto& par);
		std::string handle_post_include(const auto& req, const auto& par);
		std::string handle_post_exclude(const auto& req, const auto& par);
		std::string err_to_response(RepositoryErr e);

		// data members
		std::shared_ptr<IRepository> repository;
		std::unique_ptr<restinio::router::express_router_t<>> router;
	};
}