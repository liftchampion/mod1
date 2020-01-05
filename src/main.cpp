#include "vulkaner.h"

int main()
{
  Vulkaner app(800, 600);

  app.init();

  try {
	app.run();
  } catch (const std::exception& e) {
	std::cerr << e.what() << std::endl;
	return 1;
  }

  return 0;
}


