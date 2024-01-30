#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"
#include "env.hpp"

#define DEFAULT_CONF "conf/sample.conf"

class Config {
   private:
	std::vector<Server> _servers;
	std::string _filePath;

	int _bracket_count;

   public:
	Config();
	~Config();
	Config(const Config &copy);
	Config &operator=(const Config &copy);

	std::ifstream _fileStream;

	void setServers(int argc, const char *argv[]);
	void setFilePath(std::string file_path);

	const std::vector<Server> &getServers() const;
	const std::string &getFilePath() const;

	void parseConfig(int argc, const char *argv[]);
	void readFile();
	void parseFile();
	void parseLine(std::string &line);
	void removeUnwanted(std::string &line);
	void removeComments(std::string &line);
};

#endif
