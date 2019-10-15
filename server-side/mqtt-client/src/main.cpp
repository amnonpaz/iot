#include "Utils.hpp"

#include "MqttClient.hpp"

#include <string>
#include <array>
#include <algorithm>
#include <map>
#include <iostream>
#include <iomanip>

class Config {
    private:
        struct Argument {
            std::string name;
            std::string desc;
            bool unary;
            bool null;
        };

        typedef std::array<Argument, 3> ArgumentsList;
        static const ArgumentsList arguments;

    public:
        Config() {};
        void parse(int argc, const char *argv[]);

        const std::string &get(const std::string &arg);
        bool exists(const std::string &arg);
        static void printArguments();

    private:
        const Argument &findArgByName(const std::string &argName);

    private:
        std::map<std::string, std::string> m_config;
};

const Config::ArgumentsList Config::arguments{
    Config::Argument{"url",  "Broker URL", false , false},
    Config::Argument{"port", "Broker port" , false, false},
    Config::Argument{"help", "Print help message" , true, false}
};

const Config::Argument &Config::findArgByName(const std::string &argName)
{
    static const Argument NullArg = { "", "", false, true };

    auto itr = std::find_if(arguments.begin(), arguments.end(),
                    [&argName](const Argument &arg){ return arg.name == argName; });

    return (itr != arguments.end()) ? *itr : NullArg;
}

void Config::parse(int argc, const char *argv[])
{
    const static std::string prefix{"--"};

    for (int i = 1; i < argc; i++) {
        std::string token(argv[i]);
        if (token.compare(0, prefix.length(), prefix) != 0) {
            throw std::runtime_error("Invalid input: \"" + token + "\"");
        }

        auto eq = token.find('=', prefix.length());
        std::string var{token, prefix.length(), eq - (prefix.length())};

        const Argument &arg = findArgByName(var);
        if (arg.null) {
            throw std::runtime_error("Invalid argument: \"" + arg.name + "\"");
        }

        std::string value{""};
        if (eq != std::string::npos) {
            if (arg.unary) {
                throw std::runtime_error("Argument \"" + arg.name + "\" cannot take a value");
            }

            if (eq == token.length() - 1) {
                throw std::runtime_error("No value for \"" + arg.name);
            }

            value = std::string{token, eq + 1};
        } else if (!arg.unary) {
            throw std::runtime_error("Argument \"" + arg.name + "\" must take a value");
        }

        m_config[var] = value;
    }
}

const std::string &Config::get(const std::string &arg) {
    static const std::string def{""};

    auto it = m_config.find(arg);
    if (it == m_config.end()) {
        return def;
    }

    return it->second;
}

bool Config::exists(const std::string &arg) {
    return m_config.find(arg) != m_config.end();
}

void Config::printArguments() {
    ::size_t max = 0;

    for (const Argument &arg : arguments) {
        max = (max > arg.name.length()) ? max : arg.name.length();
    }

    max++;

    for (const Argument &arg : arguments) {
        std::cout << "    " << std::left << std::setw(max) << arg.name;
        std::cout << std::right << std::setw(max + arg.desc.length()) << arg.desc << '\n';
    }
}

int main(int argc, const char *argv[])
{
    Config config;

    try {
        config.parse(argc, argv);
    } catch (const std::exception &e) {
        std::cerr << "Error parsing command line arguments:\n";
        std::cerr << "    " << e.what() << '\n';
        return 1;
    }

    if (config.exists("help")) {
        std::cout << "Data collector MqTT client\n";
        std::cout << "Options:\n";
        Config::printArguments();
        return 0;
    }

    return 0;
}
