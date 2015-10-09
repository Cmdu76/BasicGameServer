#ifndef SERVER_HPP
#define SERVER_HPP

#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <functional>

#include <SFML/Network.hpp>
#include <SFML/System.hpp>

#include "Peer.hpp"

class Server
{
    public:
        Server(std::string const& propertiesFile, std::string const& logFile = "");
        ~Server();

        typedef std::function<std::string(std::string)> Command;
        typedef std::function<void(sf::Packet&,Peer&)> Response;

        // Server
        virtual void start();
        virtual void stop();
        bool isRunning() const;

        // Packet
        void sendToAll(sf::Packet& packet);
        void sendToPeer(sf::Packet& packet, sf::Uint32 peerId);
        void sendToPeer(sf::Packet& packet, Peer& peer);
        void sendToPeer(sf::Packet& packet, std::string const& username);
        bool isConnected(std::string const& username);

        // Commands
        void handleAdminInput();
        std::string handleCommand(std::string const& command, bool server = true, std::string const& username = "");
        static std::vector<std::string> splitArguments(std::string const& command);

        // Admin
        bool isAdmin(std::string const& username);
        void addAdmin(std::string const& username);
        void removeAdmin(std::string const& username);
        void loadAdmins(std::string const& adminFile = "admin.txt");
        void saveAdmins(std::string const& adminFile = "admin.txt");

        // Ban
        bool isBanned(std::string const& username);
        bool isBannedIp(sf::IpAddress const& ip);
        void ban(std::string const& username, std::string const& reason = "");
        void banIp(sf::IpAddress const& ip, std::string const& reason = "");
        void unban(std::string const& username);
        void unbanIp(sf::IpAddress const& ip);
        void loadBans(std::string const& banFile = "ban.txt");
        void saveBans(std::string const& banFile = "ban.txt");

        // Settings
        std::string getSettings(std::string const& id) const;
        void setSettings(std::string const& id, std::string const& value);
        bool loadSettings();
        bool saveSettings();
        virtual void createSettings();

        // Output
        static std::string getTimeFormat();

        // Log
        bool openLog(std::string const& logFile = "");
        bool isLogOpen() const;
        std::ofstream& getLogStream();

    protected:
        virtual void initCommands();
        virtual void initPacketResponses();

        void setListening(bool enable);

        void run();
        void update(sf::Time dt);

        void handlePackets();
        void handlePacket(sf::Packet& packet, Peer& peer, bool& detectedTimeout);

        void handleConnections();
        void handleDisconnections();

    protected:
        sf::Thread mThread;
        bool mRunning;

        sf::TcpListener mListener;
        bool mListeningState;

        std::map<std::string,Command> mCommands;
        std::map<std::string,bool> mPermissions; // true = possible for non-admin user
        std::vector<std::string> mAdmins;

        std::vector<std::string> mBannedUsers;
        std::vector<sf::IpAddress> mBannedIps;

        std::map<sf::Int32,Response> mPacketResponses;

        std::map<std::string,std::string> mSettings;
        std::string mSettingsFile;

        std::ofstream mLog;

        std::size_t mMaxPlayers;
        std::size_t mPort;
        sf::Time mClientTimeoutTime;
        sf::Time mUpdateInterval;

        std::size_t mConnectedPlayers;

        std::vector<Peer::Ptr> mPeers;
};

Server& operator<<(Server& server, std::string const& v);

#endif // SERVER_HPP
