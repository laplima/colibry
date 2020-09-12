#include <iostream>
#include "../InteractiveShell.h"

using namespace std;
using namespace colibry;

namespace Cmd {
	DECLARE_ISHELL_FUNC(comando1);
	DECLARE_ISHELL_FUNC(comando2);
	DECLARE_ISHELL_FUNC(zero);
	DECLARE_ISHELL_FUNC(exit);
	DECLARE_ISHELL_FUNC(help);
};

struct Context {
	int x,y;
	Groups gs;
};

int main(int argc, char* argv[])
{
	InteractiveShell* ish = InteractiveShell::Instance();

	Context cxt = {12,34};
	ish->SetCtx(&cxt);

	ish->RegisterCmds()
		("cmd1",Cmd::comando1,"this is one",{"g1","g2"})
		("cmd2",Cmd::comando2,"this is two",{"g2"})
		("zero",Cmd::zero,"This is zero",{"g3"})
		("exit",Cmd::exit,"exit program")
		("help",Cmd::help,"list commands");

	ish->SetArg0Options("zero", {"bala","km","açucar"});

	string cmd;
	do {
		try {
			cmd = ish->ReadExec("> ");
		} catch (const EmptyLine&) {
		} catch (const UnknownCmd& uce) {
			cerr << "\tUnknown command: " << uce.what() << endl;
		} catch (const SyntaxError& s) {
			cerr << ish->Doc(s.what()) << endl;
		}
	} while (cmd != "exit");

    return 0;
}

ISHELL_FUNC_FULL(Cmd::comando1,tks,context)
{
	cout << "UM!!" << endl;
	Context* ctx = static_cast<Context*>(context);
	cout << "(" << ctx->x << "," << ctx->y << ")" << endl;
	ctx->x += 1;
	ctx->y += 1;
	ctx->gs.clear();
	ctx->gs.push_back("g2");
}

ISHELL_FUNC_FULL(Cmd::comando2,tks,context)
{
	cout << "DOIS!!" << endl;
	Context* ctx = static_cast<Context*>(context);
	cout << "(" << ctx->x << "," << ctx->y << ")" << endl;
	ctx->x += 2;
	ctx->y += 2;
	ctx->gs.clear();
	ctx->gs.push_back("g3");
}

ISHELL_FUNC_FULL(Cmd::zero,args,ctx)
{
	cout << "zero" << endl;
	Context* ctxt = static_cast<Context*>(ctx);
	cout << "(" << ctxt->x << "," << ctxt->y << ")" << endl;
	ctxt->x += 3;
	ctxt->y += 3;
	ctxt->gs.clear();
	ctxt->gs.push_back("g1");
}

ISHELL_FUNC_NOARGS(Cmd::exit)
{
	cout << "exiting" << endl;
}

ISHELL_FUNC_FULL(Cmd::help,tks,context)
{
	Context* ctx = static_cast<Context*>(context);
	InteractiveShell::Instance()->Help(ctx->gs);
}
