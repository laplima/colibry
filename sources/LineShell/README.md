# LINE SHELL

## A command-line completer/interpreter based on linenoise

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
	PersistenceManager::load(sh,"cmds.json");
	PersistenceManager::load(sh,R"(
     	// include JSON string here
	)");

	//sh.add_cmd(Command::create("list")
	//	.described_as("list everything")
	//	);

	sh.set_prompt("=> ");
	sh.cmdloop();
```
