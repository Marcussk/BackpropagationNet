#ifndef BPNETWORK_LOGGER_H
#define BPNETWORK_LOGGER_H

#include <string>
#include <iostream>

class Logger
{
public:
	~Logger() {
	}

	static Logger* getInstance() {
		if(!m_loggerInstance) {
			m_loggerInstance = new Logger();
		}
		return m_loggerInstance;
	}

	void error(std::string text) {
		std::cerr << text << std::endl;
		exit(1);
	}

	void debug(std::string text) {
		if (m_verbosity > 0) {
			std::cout << text << std::endl;
		}
	}

	void trace(std::string text) {
		if (m_verbosity > 1) {
		std::cout << text << std::endl;
		}
	}

	void verbose(std::string text) {
		if (m_verbosity > 2) {
			std::cout << text << std::endl;
		}
	}

	void out(std::string text) {
		std::cout << text << std::endl;
	}

	void setVerbosity(int verbosity) {
		m_verbosity = verbosity;
	}

private:
	Logger()
	{
	}

	int m_verbosity = 0;
	static Logger* m_loggerInstance;
};

#endif
