#include "ezop.h"
#include <discord_register.h>
#include <discord_rpc.h>
#include <iostream>
#include <filesystem>
#include <vector>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>
#include <Windows.h>

class Discord {
	public:
		Discord(ez::ezOptionParser& ezop) : ezObj(ezop) {
			if (this->ezObj.isSet("-f")) {
				std::cout << "[NOTE] JSON data provided" << std::endl;
				this->ezObj.get("-f")->getString(this->filePath);
				this->jsonObj = nlohmann::json::parse(this->readFile(this->filePath));
			}
			memset(&this->Handle, 0, sizeof(this->Handle));
			memset(&this->discordPresence, 0, sizeof(this->discordPresence));
			this->time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			this->init(); this->update();
			std::thread(this->background).join(); // stops main thread until user ends joined thread
		}
		enum errors : unsigned char { E_NO_ID = 0xF1,   E_NO_STATE,        E_NO_DETAILS,
                                              E_NO_IMAGEKEY_L,  E_NO_IMAGETEXT_L,  E_NO_IMAGEKEY_S,
					      E_NO_IMAGETEXT_S, E_JSON_TYPE_ERROR, E_FILE_NO_EXIST };
	private:
		void init();
		void update();
		static void background();
		void performChecks();
		void parseJSON();
		std::string readFile(const std::string inFile);
		int64_t time;
		DiscordEventHandlers Handle; DiscordRichPresence discordPresence;
		ez::ezOptionParser ezObj;
		nlohmann::json jsonObj; std::vector<std::string> jItems;
		std::string filePath; std::string appID;
};

inline void Discord::init() {
	if (!this->ezObj.isSet("-f")) {
		if (this->ezObj.isSet("-id")) {
			this->ezObj.get("-id")->getString(this->appID);
			if (!this->appID.empty()) {
				Discord_Initialize(this->appID.c_str(), &this->Handle, 1, NULL); // something provided, pass
				std::cout << "applicationID: " << this->appID << std::endl;
			}
			else {
				throw this->E_NO_ID; // empty provided
			}
		}
		else {
			throw this->E_NO_ID; // not even set >:(
		}
	}
	else {
		this->parseJSON();
		this->appID = this->jItems[0];
		if (!this->appID.empty()) {
			Discord_Initialize(this->appID.c_str(), &this->Handle, 1, NULL);
			std::cout << "applicationID: " << this->appID << std::endl;
		}
		else {
			throw this->E_NO_ID;
		}
	}
}

inline void Discord::update() {
	this->discordPresence.startTimestamp = this->time;

	if (!this->ezObj.isSet("-f")) {
		if (this->ezObj.isSet("-s")) {
			this->ezObj.get("-s")->getCString(&this->discordPresence.state);
		}
		else {
			throw this->E_NO_STATE;
		}
		if (this->ezObj.isSet("-d")) {
			this->ezObj.get("-d")->getCString(&this->discordPresence.details);
		}
		else {
			throw this->E_NO_DETAILS;
		}
		if (this->ezObj.isSet("-imgL")) {
			this->ezObj.get("-imgL")->getCString(&this->discordPresence.largeImageKey);
		}
		else {
			throw this->E_NO_IMAGEKEY_L;
		}
		if (this->ezObj.isSet("-imgLText")) {
			this->ezObj.get("-imgLText")->getCString(&this->discordPresence.largeImageText);
		}
		else {
			throw this->E_NO_IMAGETEXT_L;
		}
		if (this->ezObj.isSet("-imgS")) {
			this->ezObj.get("-imgS")->getCString(&this->discordPresence.smallImageKey);
		}
		else {
			throw this->E_NO_IMAGEKEY_S;
		}
		if (this->ezObj.isSet("-imgSText")) {
			this->ezObj.get("-imgSText")->getCString(&this->discordPresence.smallImageText);
		}
		else {
			throw this->E_NO_IMAGETEXT_S;
		}
	}
	else {
		this->discordPresence.details = this->jItems[1].c_str();
		this->discordPresence.state = this->jItems[2].c_str();
		this->discordPresence.largeImageKey = this->jItems[3].c_str();
		this->discordPresence.largeImageText = this->jItems[4].c_str();
		this->discordPresence.smallImageKey = this->jItems[5].c_str();
		this->discordPresence.smallImageText = this->jItems[6].c_str();
	}

	if (this->ezObj.isSet("-join")) {
		this->discordPresence.partyId = "ae488379-351d-4a4f-ad32-2b9b01c91657"; std::cout << "partyID: " << this->discordPresence.partyId << std::endl;
		this->discordPresence.joinSecret = "MTI4NzM0OjFpMmhuZToxMjMxMjM=";      std::cout << "joinSecret: " << this->discordPresence.joinSecret << std::endl;
	}

	this->performChecks();
}

// function that allows user to hide and show the console, as well as break the joined thread--killing the program.
inline void Discord::background() {
	std::cout << "\nPress [ENTER] to background this window." << std::endl;
	std::cout << "  - CTRL + INSERT :show window\n  - CTRL + HOME   :hide window\n  - F1   + F10    :kill" << std::endl;
	getchar(); ShowWindow(GetConsoleWindow(), SW_HIDE);
	while (true) {
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState(VK_INSERT) & 0x8000) {
			ShowWindow(GetConsoleWindow(), SW_SHOW);
		}
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState(VK_HOME) & 0x8000) {
			ShowWindow(GetConsoleWindow(), SW_HIDE);
		}
		if (GetAsyncKeyState(VK_F1) & 0x8000 && GetAsyncKeyState(VK_F10) & 0x8000) {
			break; // ends joined thread found in Discord::Discord(...) at std::thread(this->background).join();
				   // Returns control to main thread.
		}
		Sleep(75);
	}
}

inline void Discord::performChecks() {
	// state checks
	if ((static_cast<std::string>(this->discordPresence.state)).empty()) {
		throw this->E_NO_STATE;
	} 
	else {
		std::cout << "state: " << this->discordPresence.state << std::endl;
	}

	// details checks
	if ((static_cast<std::string>(this->discordPresence.details)).empty()) {
		throw this->E_NO_DETAILS;
	}
	else {
		std::cout << "details: " << this->discordPresence.details << std::endl;
	}

	// largeImageKey checks
	if ((static_cast<std::string>(this->discordPresence.largeImageKey)).empty()) {
		throw this->E_NO_IMAGEKEY_L;
	}
	else {
		std::cout << "largeImageKey: " << this->discordPresence.largeImageKey << std::endl;
	}
	
	// largeImageText checks
	if ((static_cast<std::string>(this->discordPresence.largeImageText)).empty()) {
		throw this->E_NO_IMAGETEXT_L;
	}
	if ((static_cast<std::string>(this->discordPresence.largeImageText)).length() > 128) {
		this->jItems[4].resize(128); this->discordPresence.largeImageText = this->jItems[4].c_str();
		std::cout << "largeImageText [TRUNC]: " << this->discordPresence.largeImageText << std::endl;
	}
	else {
		std::cout << "largeImageText: " << this->discordPresence.largeImageText << std::endl;
	}

	// smallImageKey checks
	if ((static_cast<std::string>(this->discordPresence.smallImageKey)).empty()) {
		throw this->E_NO_IMAGEKEY_S;
	}
	else {
		std::cout << "smallImageKey: " << this->discordPresence.smallImageKey << std::endl;
	}

	// smallImageText checks
	if ((static_cast<std::string>(this->discordPresence.smallImageText)).empty()) {
		throw this->E_NO_IMAGETEXT_S;
	}
	if ((static_cast<std::string>(this->discordPresence.smallImageText)).length() > 128) {
		this->jItems[6].resize(128); this->discordPresence.smallImageText = this->jItems[6].c_str();
		std::cout << "smallImageText [TRUNC]: " << this->discordPresence.smallImageText << std::endl;
	}
	else {
		std::cout << "smallImageText: " << this->discordPresence.smallImageText << std::endl;
	}

	Discord_UpdatePresence(&this->discordPresence);
}

inline std::string Discord::readFile(const std::string inFile) {
	if (std::filesystem::exists(inFile)) {
		std::ifstream t(inFile);
		std::stringstream buffer;
		buffer << t.rdbuf();
		return buffer.str();
	}
	else {
		throw this->E_FILE_NO_EXIST;
	}
}

inline void Discord::parseJSON() {
	try {
		this->jItems.push_back(this->jsonObj["applicationID"].get<std::string>());
		this->jItems.push_back(this->jsonObj["details"].get<std::string>());
		this->jItems.push_back(this->jsonObj["state"].get<std::string>());
		this->jItems.push_back(this->jsonObj["largeImageKey"].get<std::string>());
		this->jItems.push_back(this->jsonObj["largeImageText"].get<std::string>());
		this->jItems.push_back(this->jsonObj["smallImageKey"].get<std::string>());
		this->jItems.push_back(this->jsonObj["smallImageText"].get<std::string>());
	}
	catch (nlohmann::detail::type_error) {
		throw this->E_JSON_TYPE_ERROR;
	}
}
