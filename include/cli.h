#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <initializer_list>

namespace cli {

struct Option {
	enum class Type {
		Flag,
		FlagCount,
		Int,
		Float,
		String,
		Path,
		PathExisting
	};
	Type type;
	char shortName;
	const char* longName;
	const char* description;
	bool isRequired;
	
	bool isSet;
	void* valuePointer;

	bool requiresParameter() const {
		return type != Option::Type::Flag && type != Option::Type::FlagCount;
	}

	template<typename T> 
	T& as() const {
		return *static_cast<T*>(valuePointer);
	}
};

#ifndef CLI_LOG_ERROR
#define CLI_LOG_ERROR(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#endif

#ifndef CLI_LOG_USAGE
#define CLI_LOG_USAGE(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#endif

#ifndef CLI_LOG_INFO
#define CLI_LOG_INFO(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#endif

static Option OptionFlag(char shortName, const char* longName, const char* description, bool* valuePointer){
	return Option {Option::Type::Flag, shortName, longName, description, false, false, valuePointer};
}

static Option OptionFlagCount(char shortName, const char* longName, const char* description, int* valuePointer){
	return Option {Option::Type::FlagCount, shortName, longName, description, false, false, valuePointer};
}

static Option OptionInt(char shortName, const char* longName, const char* description, bool required, int* valuePointer){
	return Option {Option::Type::Int, shortName, longName, description, required, false, valuePointer};
}

static Option OptionFloat(char shortName, const char* longName, const char* description, bool required, float* valuePointer){
	return Option {Option::Type::Float, shortName, longName, description, required, false, valuePointer};
}

static Option OptionString(char shortName, const char* longName, const char* description, bool required, const char** valuePointer){
	return Option {Option::Type::String, shortName, longName, description, required, false, valuePointer};
}

static Option OptionPath(char shortName, const char* longName, const char* description, bool required, const char** valuePointer){
	return Option {Option::Type::Path, shortName, longName, description, required, false, valuePointer};
}

static Option OptionPathExisting(char shortName, const char* longName, const char* description, bool required, const char** valuePointer){
	return Option {Option::Type::PathExisting, shortName, longName, description, required, false, valuePointer};
}

class Parser {
public:
	Parser(std::initializer_list<Option> options) : options(options), executableName(NULL) {}

	bool parse(int argc, const char* argv[]) {
		executableName = argv[0];

		for(int i = 1; i < argc; ++i) {
			int result = handleOption(argc-i,argv+i);
			// error
			if(result < 0) return false;
			// skip any consumed parameters
			i+= result;
		}
		for(Option& opt : options) {
			if(opt.isRequired && !opt.isSet) {
				CLI_LOG_ERROR("error: option -%c/--%s is required\n", opt.shortName, opt.longName);
				return false;
			}
		}
		return true;
	}

	bool validatePathOptions() {
		bool valid = true;
		for(Option& opt: options) {
			if(opt.type == Option::Type::PathExisting && !checkExistsReadable(opt.as<const char*>())) {
				valid = false;
				CLI_LOG_ERROR("error: option -%c/--%s requires a readable file\n", opt.shortName, opt.longName);
			}
		}
		return valid;
	}

	void printOptionsUsage() {
		CLI_LOG_USAGE("Options:\n");
		for(Option& opt : options) {
			if(opt.requiresParameter()) {
				CLI_LOG_USAGE("  -%c, --%s <%s>\t%s", opt.shortName, opt.longName, optionTypeDisplayName(opt.type), opt.description);
			} else {
				CLI_LOG_USAGE("  -%c, --%s\t%s", opt.shortName, opt.longName, opt.description);
			}
			if(opt.isRequired) {
				CLI_LOG_USAGE(" (required)");
			}
			CLI_LOG_USAGE("\n");
		}		
	}

	const std::vector<const char*>& getRemainingArgs() const {
		return remaining;
	}

private:
	std::vector<Option> options;
	std::vector<const char*> remaining;
	const char* executableName;

	int applyOption(Option& opt, int argc, const char** argv) {
		if(opt.type != Option::Type::FlagCount && opt.isSet) {
			CLI_LOG_ERROR("error: option -%c/--%s shouldn't be specified more than once\n", opt.shortName, opt.longName);
			return -1;
		}
		opt.isSet = true;
		// Other types expect an argument
		if(opt.requiresParameter() && argc < 2) {
			CLI_LOG_ERROR("error: option -%c/--%s requires a parameter\n", opt.shortName, opt.longName);
			return -1;
		}

		const char* argParam = argv[1];

		switch(opt.type) {
			case Option::Type::Flag:
				opt.as<bool>() = true;
				return 0;
			case Option::Type::FlagCount:
				opt.as<int>()++;
				return 0;
			case Option::Type::Int:
				if(!isNumeric(argParam, false)) {
					CLI_LOG_ERROR("error: invalid integer value \"%s\" specified for option -%c/--%s\n", argParam, opt.shortName, opt.longName);
					return -1;
				}
				opt.as<int>() = atoi(argParam);
				return 1;
			case Option::Type::Float:
				if(!isNumeric(argParam, true)) {
					CLI_LOG_ERROR("error: invalid float value \"%s\" specified for option -%c/--%s\n", argParam, opt.shortName, opt.longName);
					return -1;
				}
				opt.as<float>() = atof(argParam);
				return 1;
			case Option::Type::String:
			case Option::Type::Path:
			case Option::Type::PathExisting:
				opt.as<const char*>() = argParam;
				return 1;
		}
		return 0;
	}

	bool isNumeric(const char* str, bool floatingPoint) {
		bool beginExponent = false;
		bool foundDecimal = false;
		for(int i = 0; i < strlen(str); ++i) {
			switch(str[i]) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					continue;
				case '-':
				case '+':
					if(i == 0 || (floatingPoint && beginExponent)) {
						continue;
					}
					return false;
				case '.':
					if(floatingPoint && !foundDecimal) {
						foundDecimal = true;
						continue;
					}
					return false;
				case 'e':
				case 'E':
					if(floatingPoint) {
						beginExponent = true;
						floatingPoint = false;
					}
					return false;
				default:
					return false;
			}
		}
		return true;
	}

	int handleOption(int argc, const char** argv) {
		const char* arg = argv[0];
		if(strlen(arg) > 1 && arg[0] == '-') {
			// Handle concatenated short options
			if(arg[1] != '-') {
				for(int i = 1; i < strlen(arg); ++i) {
					bool handled = false;
					for(Option& opt : options) {
						if(opt.shortName == arg[i]) {
							// arguments requiring parameters can't be in the middle of the list
							if(opt.requiresParameter() && i < strlen(arg) - 1) {
								CLI_LOG_ERROR("error: short option -%c cannot be used in the middle of a flag list, it requires a value\n", opt.shortName);
								return -1;
							}
							handled = true;
							int result = applyOption(opt, argc, argv);
							if(result != 0) return result;
						}
					}
					if(!handled) {
						CLI_LOG_ERROR("error: unknown short option -%c\n", arg[i]);
						return -1;
					}
				}
			} else {
				for(Option& opt : options) {
					if(strcmp(arg+2, opt.longName) == 0) {
						return applyOption(opt, argc, argv);
					}
				}
				CLI_LOG_ERROR("error: unknown option %s\n", argv[0]);
				return -1;
			}
		} else {
			remaining.push_back(arg);
		}
		return 0;
	}

	const char* optionTypeDisplayName(Option::Type type) {
		switch(type) {
			case Option::Type::Flag:
			case Option::Type::FlagCount:
				return "flag";
			case Option::Type::Int:
				return "integer";
			case Option::Type::Float:
				return "float";
			case Option::Type::String:
				return "string";
			case Option::Type::Path:
			case Option::Type::PathExisting:
				return "path";
			default:
				return "unknown";
		}
	}

	bool checkExistsReadable(const char* path) {
		FILE* f = fopen(path, "rb");
		if(f != NULL) {
			fclose(f);
			return true;
		} else {
			return false;
		}
	}

};

};