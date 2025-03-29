#include "GLCore.h"
#include "ExampleLayer.h"

using namespace GLCore;

class Breakout : public Application
{
public:
	Breakout()
		: Application("Breakout")
	{
		PushLayer(new ExampleLayer());
	}
};

int main()
{
	std::unique_ptr<Breakout> app = std::make_unique<Breakout>();
	app->Run();
}