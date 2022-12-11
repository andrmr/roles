## Role inclusion app

Async REST server, based on [restinio](https://github.com/Stiffstream/restinio) for HTTP and [nlohmann](https://github.com/nlohmann/json) for JSON.  
Compiled and tested with VS 2022 17.5 with /std:c++ latest.  
  
### Requirements:
* Produce a list of all the Roles and their included roles.
* Include Role in another Role.
* Delete an included role from a role.
* Simulate the deletion of a role, in order to obtain a report of the roles that are affected by this deletion(they have this role included).
* When including / deleting a role to / from a role, a response about the status of the operation should be sent.
