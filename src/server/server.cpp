#include <restinio/all.hpp>

#include <functional>
#include <ranges>

#include "server.h"

using namespace tser;

Server::Server(std::shared_ptr<IRepository> repository)
	: repository { repository }
	, router { std::make_unique<restinio::router::express_router_t<>>() }
{
}

void
Server::run(std::string address, uint16_t port)
{
	add_all_paths();

	struct server_traits : public restinio::default_single_thread_traits_t {
		using request_handler_t = restinio::router::express_router_t<>;
		using logger_t = restinio::single_threaded_ostream_logger_t;
	};

	restinio::run(
		restinio::on_thread_pool<server_traits>(std::thread::hardware_concurrency())
			.address(std::move(address))
			.port(port)
			.request_handler(std::move(router))
	);
}

auto
Server::prepare_response(auto&& res) {
	res
		.append_header("Server", "RESTinio server")
		.append_header_date_field()
		.append_header("Content-Type", "application/json");

	return res;
}

void
Server::add_all_paths()
{
	using enum Verb;

	add_path(
		PUT,
		"/v1/api/add",
		[this](const auto&... args) {
			return handle_add_role(args...);
		}
	);

	add_path(
		GET,
		"/v1/api/roles",
		[this](const auto&... args) {
			return handle_get_roles(args...);
		}
	);

	add_path(
		GET,
		"/v1/api/simulate/:role",
		[this](const auto&... args) {
			return handle_get_simulation(args...);
		}
	);

	add_path(
		POST,
		"/v1/api/include/:role/:subrole",
		[this](const auto&... args) {
			return handle_post_include(args...);
		}
	);

	add_path(
		POST,
		"/v1/api/exclude/:role/:subrole",
		[this](const auto&... args) {
			return handle_post_exclude(args...);
		}
	);
}

void Server::add_path(Verb verb, std::string_view path, auto&& handler)
{
	static std::unordered_map<Verb, restinio::http_method_id_t(*)()> method_handlers {
		{ Verb::GET,	&restinio::http_method_get },
		{ Verb::POST,	&restinio::http_method_post },
		{ Verb::PUT,	&restinio::http_method_put },
	};

	// todo: check Content-Type for handlers which expect JSON bodies
	router->add_handler(
		std::move(method_handlers.at(verb)()),
		path,
		[=](auto req, auto par) {
			return prepare_response(req->create_response())
				.set_body(handler(req, par))
				.done();
		});
}

std::string
Server::handle_add_role(const auto& req, const auto& par)
{
	try {
		auto role = nlohmann::json::parse(req->body());
		const auto result = repository->add_role(std::move(role));

		return err_to_response(result);
	}
	catch (std::exception& e) {
		nlohmann::json j;
		j["success"] = false;
		j["reason"] = e.what();
		return j.dump();
	}	
}

std::string
Server::handle_get_roles(const auto& req, const auto& par)
{
	const auto roles = repository->roles();

	nlohmann::json j;
	j["success"] = true;
	if (roles.empty()) {
		j["roles"] = {};
	}
	else {
		for (const auto& role : std::ranges::views::values(roles)) {
			j["roles"].push_back(role);
		}
	}

	return j.dump();
}

std::string
Server::handle_get_simulation(const auto& req, const auto& par)
{
	const auto roles = repository->dependencies(Role::Uuid(par["role"]));

	nlohmann::json j;

	if (!roles) {
		j["success"] = false;
		j["reason"] = repository->err_to_str(roles.error());
	}
	else {
		j["success"] = true;
		if (roles.value().has_value()) {
			j["deps"] = *roles.value();
		}
		else {
			j["deps"] = {};
		}
	}

	return j.dump();
}

std::string
Server::handle_post_include(const auto& req, const auto& par)
{
	const auto result = repository->include_role(
		Role::Uuid(par["role"]),
		Role::Uuid(par["subrole"]));

	return err_to_response(result);
}

std::string
Server::handle_post_exclude(const auto& req, const auto& par)
{
	const auto result = repository->exclude_role(
		Role::Uuid(par["role"]),
		Role::Uuid(par["subrole"]));

	return err_to_response(result);
}

std::string Server::err_to_response(RepositoryErr e)
{
	nlohmann::json j;
	if (e != RepositoryErr::OK) {
		j["success"] = false;
		j["reason"] = repository->err_to_str(e);
	}
	else {
		j["success"] = true;
	}

	return j.dump();
}
