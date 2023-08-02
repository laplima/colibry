# ORBManager -- Typical Usage

### SERVER

```c++
ORBManager om{argc,argv}; // default name is "ORB"
om.activate_rootpoa();
Object_i obji;
auto ref = om.activate_object<Object>(obji);	// auto = T_var
cout << om.object_to_string(ref) << endl;
om.run();

// (with <ENTER> to quit, instead of `om.run()`)

 thread orbth{[&om]() { om.run(); }};
 cout << "Press ENTER to quit." << endl;
 cin.get();
 om.shutdown();	// end CORBA event loop in the thread
 orbth.join();
```
### CLIENT

```c++
ORBManager om{argc,argv};
auto ref = om.string_to_object<T>("file://...."); // auto = T_var
ref->remote_method();
```

### Server with a child POA

```c++
ORBManager orb{argc,argv};
orb.activate_rootpoa();

MPOA rpoa = orb.root_poa();
MPOA cpoa = rpoa.create_child_poa("cpoa", {POAPolicy::USER_ID,
                               POAPolicy::NO_IMPLICIT_ACTIVATION});
Object_i obj;
cpoa.activate_object_with_id("name",obj);
```



### Notes

```c++
ORBManager om;	// non-initialized om: requires late init() before usage
om.init(argc, argv);
```

ORB-name is optional, but if the same name is provided, the same ORB is used (unless it has been previously destroyed).
