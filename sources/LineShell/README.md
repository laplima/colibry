# LINE SHELL

## A command-line completer/interpreter based on linenoise

Important: `fmt` library is required in order to compile

### Define observer class

```cpp
	using colibry::lineshell::Stringv;

	class MyCommands : public CmdObserver {
	public:
		MyCommands()
		{
			bind()
				("list", FWRAP(make_list));
		}
	
		// "help" and "exit" commands included by default
	
		// optional override
		void exit_(const Stringv& args) override {
			print("terminating\n");
			CmdObserver::exit_(args);
		}
	
		void make_list(const Stringv& args)
		{
			// args[0] is the command name
			print("LIST\n>>{}\n", fmt::join(args.begin(), args.end(), ", "));
		}
	};
```

### Main

```cpp
	using lineshell::PersistenceManager;

	MyCommands cmds;			// observer
	
	LineShell sh{cmds};			// subject
	PersistenceManager::load_file(sh,"cmds.json");
	PersistenceManager::load_str(sh,R"(
     	// include JSON string here
	)");

	//sh.add_cmd(Command::create("list")
	//	.described_as("list everything")
	//	);

	sh.set_prompt("=> ");
	sh.cmdloop();
```

## JSON Command Specification

Sample:

```json
[
	{
		"list": {
			"desc": "list",
			"args": [
				{
					"students": {
						"desc": "list students",
						"args": []
					}
				},
				{
					"projects": {
						"desc": "list projects",
						"args": [
							{
								"open" : {
									"desc": "list open projects",
									"args": []
								}
							},
							{
								"closed": {
									"desc": "list closed projects",
									"args": []
								}
							}
						]
					}
				},
				{
					"supervisors": {
						"desc": "list supervisors",
						"args": []
					}
				},
				{
					"\"Simone Menezes Lima\"": {
						"desc": "student",
						"args": []
					}
				}
			]
		}
	}
]

```
