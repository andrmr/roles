#include <concepts>
#include <format>
#include <functional>
#include <iostream>
#include <ranges>
#include <span>
#include <unordered_map>

#include <restclient-cpp/restclient.h>

#include <nlohmann/json.hpp>

#include "role.h"

/// response handlers container; used to match a command with a handler
template <std::ranges::range T>
using Handlers = std::unordered_map<std::string_view, void (*)(std::string_view, T)>;

/// @brief Prints the help message
void print_help();

/// @brief Prints a response from the API
void print_response(const RestClient::Response& response);

// handlers for the commands
void handle_add(std::string_view uri, std::ranges::forward_range auto args);
void handle_roles(std::string_view uri, std::ranges::view auto);
void handle_simulate(std::string_view uri, std::ranges::view auto args);
void handle_include(std::string_view uri, std::ranges::view auto args);
void handle_exclude(std::string_view uri, std::ranges::view auto args);

int main(int argc, const char* argv[])
{
	// convert args to span of string_view for easier use
	const auto args = std::span(argv, argc)
		| std::views::transform([](char const* v) { return std::string_view(v); });

	auto args_it = args.begin();
	std::advance(args_it, 1);

	// no args
	if (args_it == args.end()) {
		print_help();
		return 0;
	}

	// todo: make handlers return some result type to report back to user
	const Handlers<decltype(std::ranges::subrange(args))> actions {
		{"roles", &handle_roles},
		{"add", &handle_add},
		{"simulate", &handle_simulate},
		{"include", &handle_include},
		{"exclude", &handle_exclude},
	};

	// handle help
	if (*args_it == "help") {
		print_help();
		return 0;
	}

	// handle regular usage sequence
	if (*args_it != "uri") {
		std::cout << "\nError: Unknown usage sequence \n\n";
		print_help();
		return 1;
	}

	// get uri
	std::advance(args_it, 1);
	if (args_it == args.end()) {
		std::cout << "\nError: URI not provided \n\n";
		print_help();
		return 1;
	}
		
	auto uri = *args_it;

	// get cmd
	std::advance(args_it, 1);
	if (args_it == args.end()) {
		std::cout << "\nError: CMD not provided \n\n";
		print_help();
		return 1;
	}

	auto cmd = *args_it;

	// execute with the rest of the args
	try {
		std::advance(args_it, 1);

		auto remaining_args = std::ranges::subrange(args_it, args.end());

		actions.at(cmd)(uri, remaining_args);
		return 0;
	}
	catch (std::out_of_range&) {
		std::cout << "\nError: Unknown command \n\n";
		print_help();
		return 1;
	}

	return 0;
}

void print_help()
{
	std::cout
		<< "Role application client \n\n"
		<< "Usage: client.exe uri <uri> <cmd> <cmd_args> -- where:\n"
		<< "\t - <uri> is the server's API URI e.g. http://localhost:80/v1/api \n"
		<< "\t - <cmd> <cmd_args> are a supported command and its args \n\n"
		<< "Example: client.exe uri http://localhost:80/v1/api roles \n\n"
		<< "Commands: \n"
		<< "\t help -- Prints this message \n"
		<< "\t add <role_id> <role_name> -- Adds a new role \n"
		<< "\t roles -- Gets all existing roles \n"
		<< "\t simulate <subrole_id> -- Returns a list of roles which include the given sub-role \n"
		<< "\t include <role_id> <subrole_id> -- Includes a sub-role in a given role \n"
		<< "\t exclude <role_id> <subrole_id> -- Excludes a sub-role from a given role \n"
		;
}

void print_response(const RestClient::Response& response)
{
	// todo: extract fields from JSON responses
	std::cout
		<< "\nCode: " << response.code
		<< "\nBody: " << response.body
		<< "\n\n";
	;
}

void handle_add(std::string_view uri, std::ranges::forward_range auto args) {
	if (args.size() != 2) {
		std::cout << "\nError: Not enough parameters\n\n";
		return;
	}

	const auto role = tser::Role(tser::Role::Name(args[1]), tser::Role::Uuid(args[0]));

	const auto full_path = std::format("{}/add", uri);
	const auto response = RestClient::put(full_path, "application/json", nlohmann::json(role).dump());

	print_response(response);
}

void handle_roles(std::string_view uri, std::ranges::view auto) {
	const auto full_path = std::format("{}/roles", uri);
	const auto response = RestClient::get(full_path);

	print_response(response);
}

void handle_simulate(std::string_view uri, std::ranges::view auto args) {
	if (args.size() != 1) {
		std::cout << "\nError: Not enough parameters\n\n";
		return;
	}

	const auto full_path = std::format("{}/simulate/{}", uri, args[0]);
	const auto response = RestClient::get(full_path);

	print_response(response);
}

void handle_include(std::string_view uri, std::ranges::view auto args) {
	if (args.size() != 2) {
		std::cout << "\nError: Not enough parameters\n\n";
		return;
	}

	const auto full_path = std::format("{}/include/{}/{}", uri, args[0], args[1]);
	const auto response = RestClient::post(full_path, "", "");

	print_response(response);
}

void handle_exclude(std::string_view uri, std::ranges::view auto args) {
	if (args.size() != 2) {
		std::cout << "\nError: Not enough parameters\n\n";
		return;
	}

	const auto full_path = std::format("{}/exclude/{}/{}", uri, args[0], args[1]);
	const auto response = RestClient::post(full_path, "", "");

	print_response(response);
}