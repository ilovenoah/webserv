#include "main.hpp"

int error_log(std::string string)
{
	std::ofstream log;
	log.open("error.log", std::ios::app);
	log << string << std::endl;
	log.close();
	std::cout << string << std::endl;
	return(1);
}

int main(int argc, char const *argv[])
{
	if (argc > 2)
		return(error_log("Too many arguments"));

}
