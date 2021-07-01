#include "discord.h"

ez::ezOptionParser ez_init(int argc, const char* argv[]);
std::string errorText(Discord::errors ecode);

int main(unsigned int argc, const char* argv[]) {
	system("cls");
	ez::ezOptionParser ezop = ez_init(argc, argv);
	try {
		Discord presence(ezop);
		// main thread stops here if Discord(...) passes all routines
		// see Discord::background and Discord::Discord(...) for more info...
	} catch (Discord::errors e) {
		std::cout << errorText(e);
		exit(e);
	}
	return EXIT_SUCCESS;
}

ez::ezOptionParser ez_init(int argc, const char* argv[]) {
	ez::ezOptionParser ezop;
	ezop.overview = "[rpresence] :: a custom Discord Rich Presence tool made by someone too lazy for a GUI";
	ezop.syntax = "(./)rpresence(.exe) [OPTIONS...]";
	ezop.footer = "\n*Maximum character limit: 128 bytes";
	ezop.add(
		"",  // default value
		0,   // required...?
		0,   // args expected
		0,   // delimiter for multiple args
		"Display usage instructions.", // description
		"-h" // token(s)...
	);
	ezop.add("", 0, 1, 0, "Discord Application ID (discord.com/developers/applications)", "-id");
	ezop.add("", 0, 1, 0, "Details of Rich Presence (text to right of images)", "-d");
	ezop.add("", 0, 1, 0, "Path to JSON file containing Rich Presence data", "-f");
	ezop.add("", 0, 1, 0, "State of Rich Presence (text shown below details)", "-s");
	ezop.add("", 0, 0, 0, "Enable/disable join button for others to see (default:false)", "-join");
	ezop.add("", 0, 1, 0, "Large image key (name of an asset under Rich Presence->Assets)", "-imgL");
	ezop.add("", 0, 1, 0, "*Large image text (shown when user hovers on image)", "-imgLText");
	ezop.add("", 0, 1, 0, "Small image key (name of an asset under Rich Presence->Assets)", "-imgS");
	ezop.add("", 0, 1, 0, "*Small image text (shown when user hovers on image)", "-imgSText");
	ezop.parse(argc, argv);

	if (ezop.isSet("-h")) {
		std::string usage; ezop.getUsage(usage, 80, ez::ezOptionParser::Layout::ALIGN);
		std::cout << usage << std::endl;
		exit(EXIT_SUCCESS);
	}

	return ezop;
}

std::string errorText(Discord::errors ecode) {
	switch (ecode) {
		case Discord::E_NO_ID:            { return "[Error] : -id not set or no parameter supplied in -id!";             }
		case Discord::E_NO_STATE:         { return "[Error] : -s not set or no parameter supplied in -s!";               }
		case Discord::E_NO_DETAILS:       { return "[Error] : -d not set or no parameter supplied in -d!";               }
		case Discord::E_NO_IMAGEKEY_L:    { return "[Error] : -imgL not set or no parameter supplied in -imgL!";         }
		case Discord::E_NO_IMAGETEXT_L:   { return "[Error] : -imgLText not set or no parameter supplied in -imgLText!"; }
		case Discord::E_NO_IMAGEKEY_S:    { return "[Error] : -imgS not set or no parameter supplied in -imgS!";         }
		case Discord::E_NO_IMAGETEXT_S:   { return "[Error] : -imgSText not set or no parameter supplied in -imgSText!"; }
		case Discord::E_JSON_TYPE_ERROR:  { return "[Error] : Malformed JSON data supplied; check file contents.";       }
		case Discord::E_FILE_NO_EXIST:    { return "[Error] : File provided does not exist; check path entered.";        }
	}
}